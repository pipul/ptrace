#include <linux/types.h>
#include <linux/module.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/spinlock_types.h>
#include <net/ip.h>

#define PCNT_VALID_HOOKS ((1 << NF_INET_LOCAL_IN) | \
			  (1 << NF_INET_PRE_ROUTING) | \
			  (1 << NF_INET_FORWARD))

static __be32 saddr_raw, daddr_raw;

static spinlock_t pcntlock;
static unsigned int packets_in, packets_forward, packets_prerouting;



/* int
 * inet_pton4(src, dst)
 *	like inet_aton() but without all the hexadecimal and shorthand.
 * return:
 *	1 if `src' is a valid dotted quad, else 0.
 * notice:
 *	does not touch `dst' unless it's returning 1.
 * author:
 *	Paul Vixie, 1996.
 */
static int
inet_pton4(const char *src, u_char *dst)
{
	static const char digits[] = "0123456789";
	int saw_digit, octets, ch;
#define NS_INADDRSZ	4
	u_char tmp[NS_INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;

		if ((pch = strchr(digits, ch)) != NULL) {
			u_int new = *tp * 10 + (pch - digits);

			if (saw_digit && *tp == 0)
				return (0);
			if (new > 255)
				return (0);
			*tp = new;
			if (!saw_digit) {
				if (++octets > 4)
					return (0);
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} else
			return (0);
	}
	if (octets < 4)
		return (0);
	memcpy(dst, tmp, NS_INADDRSZ);
	return (1);
}


static unsigned int pcnt_func(unsigned int hooknum, struct sk_buff *skb,
		   const struct net_device *in, const struct net_device *out,
		   int (*okfn)(struct sk_buff *)) {
	const struct iphdr *iph;
	const struct tcphdr *th;
	
	local_bh_disable();
	spin_lock(&pcntlock);

	iph = ip_hdr(skb);
	
	if (saddr_raw && iph->saddr != saddr_raw)
		goto out;
	if (daddr_raw && iph->daddr != daddr_raw)
		goto out;
	if (!(hooknum & PCNT_VALID_HOOKS))
		goto out;

	skb->transport_header = skb->network_header + iph->ihl * 4;
	th = tcp_hdr(skb);
 
	if (th->doff < sizeof(struct tcphdr) / 4)
		goto out;
	if (!th->psh)
		goto out;
	
	if (hooknum == NF_INET_LOCAL_IN) {
		packets_in++;
	} else if (hooknum == NF_INET_PRE_ROUTING) {
		packets_prerouting++;
	} else if (hooknum == NF_INET_FORWARD) {
		packets_forward++;
	}
	printk(KERN_INFO "prerouting:%d forward:%d in:%d\n", packets_prerouting,
	       packets_forward, packets_in);
 out:
	spin_unlock(&pcntlock);
	local_bh_enable();
	return NF_ACCEPT;
}

static struct nf_hook_ops pcnt_ops_local_in = {
	.hook = pcnt_func,
	.owner = THIS_MODULE,
	.pf = NFPROTO_IPV4,
	.hooknum = NF_INET_LOCAL_IN,
};

static struct nf_hook_ops pcnt_ops_forward = {
	.hook = pcnt_func,
	.owner = THIS_MODULE,
	.pf = NFPROTO_IPV4,
	.hooknum = NF_INET_FORWARD,
};

static struct nf_hook_ops pcnt_ops_prerouting = {
	.hook = pcnt_func,
	.owner = THIS_MODULE,
	.pf = NFPROTO_IPV4,
	.hooknum = NF_INET_PRE_ROUTING,
};



static void do_test(void) {
	spin_lock_init(&pcntlock);


	//  setting the saddr and daddr
	inet_pton4("127.0.0.2", (u_char *)&saddr_raw);
	inet_pton4("127.0.0.1", (u_char *)&daddr_raw); // match all localaddress
	
	nf_register_hook(&pcnt_ops_local_in);
	nf_register_hook(&pcnt_ops_forward);
	nf_register_hook(&pcnt_ops_prerouting);
	return;
}

static void undo_test(void) {
	nf_unregister_hook(&pcnt_ops_local_in);
	nf_unregister_hook(&pcnt_ops_forward);
	nf_unregister_hook(&pcnt_ops_prerouting);
	return;
}


static int __init init_test(void) {
	printk(KERN_INFO "init_test\n");
	do_test();
	return 0;
}

static void __exit exit_test(void) {
	printk(KERN_INFO "exit_test");
	undo_test();
	return;
}


module_init(init_test)
module_exit(exit_test)
MODULE_LICENSE("GPL");

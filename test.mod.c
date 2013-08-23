#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x14522340, "module_layout" },
	{ 0x349cba85, "strchr" },
	{ 0x973873ab, "_spin_lock" },
	{ 0xca975b7a, "nf_register_hook" },
	{ 0xea147363, "printk" },
	{ 0xb4390f9a, "mcount" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x7e5a6ea3, "nf_unregister_hook" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "FD6710DB0899F7FE8C94871");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 4,
};

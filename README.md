ptrace tool
======


准备环境
---------

需要管理员权限：sudo
安装内核开发工具必备软件：
	sudo apt-get install linux-headers-$(uname -r)
修改需要跟踪的IP：

`
	static void do_test(void) {
		spin_lock_init(&pcntlock);


		//  setting the saddr and daddr
		inet_pton4("source addr", (u_char *)&saddr_raw);
		inet_pton4("dest addr", (u_char *)&daddr_raw); // match all localaddress
	
		nf_register_hook(&pcnt_ops_local_in);
		nf_register_hook(&pcnt_ops_forward);
		nf_register_hook(&pcnt_ops_prerouting);
		return;
	}
`
该操作只需要执行一次。主要是将所有依赖安装到系统中。


编译
---------

在根目录执行 make 即可。


加载内核模块
---------
在根目录执行 sudo make ins 即可。


卸载内核模块
---------
在根目录执行 sudo make rm 即可。


查看packets统计
----------
使用 dmesg 命令

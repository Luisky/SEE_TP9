/*
 * Here's a sample kernel module showing the use of jprobes to dump
 * the arguments of _do_fork().
 *
 * For more information on theory of operation of jprobes, see
 * Documentation/kprobes.txt
 *
 * Build and insert the kernel module as done in the kprobe example.
 * You will see the trace data in /var/log/messages and on the
 * console whenever _do_fork() is invoked to create a new process.
 * (Some messages may be suppressed if syslogd is configured to
 * eliminate duplicate messages.)
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/fs.h>

/*
 * Jumper probe for do_sys_open.
 * Mirror principle enables access to arguments of the probed routine
 * from the probe handler.
 */

module_param(file_or_part, int, 0);
MODULE_PARM_DESC(file_or_part, "An int to choose if we trace a file or a part");

module_param(path_str, charp, 0);
MODULE_PARM_DESC(path_str, "A String for the path");

static struct filename *(*getname_p)(const char __user *);

/* Proxy routine having the same arguments as actual do_sys_open() routine */
long j_do_sys_open(int dfd, const char __user *filename, int flags,
		   umode_t mode)
{
	struct filename *file_n;

	// this is impossible since : http://linux-kernel.2935.n7.nabble.com/PATCH-vfs-unexport-the-getname-symbol-td766022.html
	// https://forums.centos.org/viewtopic.php?t=49653
	file_n = getname_p(filename);

	pr_info("jprobe: dfd = 0x%x, flags = 0x%x "
		"mode = 0x%x filename = %s\n",
		dfd, flags, mode, file_n->name);

	/* Always end with a call to jprobe_return(). */
	jprobe_return();
	return 0;
}

static struct jprobe my_jprobe = {
    .entry = j_do_sys_open,
    .kp = {
        .symbol_name = "do_sys_open",
    },
};

static int __init jprobe_init(void)
{
	int ret;

	pr_info("file_or_part = %d, path_str = %s", file_or_part, path_str);

	getname_p = (struct filename * (*)(const char __user *))
		kallsyms_lookup_name("getname");

	ret = register_jprobe(&my_jprobe);
	if (ret < 0) {
		pr_err("register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	pr_info("Planted jprobe at %p, handler addr %p\n", my_jprobe.kp.addr,
		my_jprobe.entry);
	return 0;
}

static void __exit jprobe_exit(void)
{
	unregister_jprobe(&my_jprobe);
	pr_info("jprobe at %p unregistered\n", my_jprobe.kp.addr);
}

module_init(jprobe_init) module_exit(jprobe_exit) MODULE_LICENSE("GPL");
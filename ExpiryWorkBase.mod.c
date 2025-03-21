#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

KSYMTAB_FUNC(IsStoppingExpiryWorkBaseFalse, "", "");
KSYMTAB_FUNC(GetExpiryWorkBasePrevious, "", "");
KSYMTAB_FUNC(LockExpiryWorkBase, "", "");
KSYMTAB_FUNC(UnlockExpiryWorkBase, "", "");
KSYMTAB_FUNC(CancelExpiryWorkBase, "", "");
KSYMTAB_FUNC(TheBenchmarksExpiryWorkBase, "", "");
KSYMTAB_FUNC(GetExpiryWorkBaseParent, "", "");
KSYMTAB_FUNC(SetupExpiryWorkBase, "", "");
KSYMTAB_FUNC(StopExpiryWorkBase, "", "");
KSYMTAB_FUNC(SetAutoDeleteExpiryWorkBase, "", "");

SYMBOL_CRC(IsStoppingExpiryWorkBaseFalse, 0x52c0a242, "");
SYMBOL_CRC(GetExpiryWorkBasePrevious, 0x8ab26e1a, "");
SYMBOL_CRC(LockExpiryWorkBase, 0x976a7284, "");
SYMBOL_CRC(UnlockExpiryWorkBase, 0x9d34c07f, "");
SYMBOL_CRC(CancelExpiryWorkBase, 0xab46d625, "");
SYMBOL_CRC(TheBenchmarksExpiryWorkBase, 0x1cbbca1b, "");
SYMBOL_CRC(GetExpiryWorkBaseParent, 0xeabf173b, "");
SYMBOL_CRC(SetupExpiryWorkBase, 0x1e170521, "");
SYMBOL_CRC(StopExpiryWorkBase, 0x158971cf, "");
SYMBOL_CRC(SetAutoDeleteExpiryWorkBase, 0x23a2ec61, "");

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x99e92706, "kmalloc_caches" },
	{ 0xe3272708, "__kmalloc_cache_noprof" },
	{ 0x2d3385d3, "system_wq" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0xf74bb274, "mod_delayed_work_on" },
	{ 0xc4f0da12, "ktime_get_with_offset" },
	{ 0x9fa7184a, "cancel_delayed_work_sync" },
	{ 0x1a79c8e9, "__x86_indirect_thunk_r13" },
	{ 0x950eb34e, "__list_del_entry_valid_or_report" },
	{ 0x37a0cba, "kfree" },
	{ 0x92997ed8, "_printk" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xffeedf6a, "delayed_work_timer_fn" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x7696f8c7, "__list_add_valid_or_report" },
	{ 0xf9108d3, "module_layout" },
};

MODULE_INFO(depends, "");


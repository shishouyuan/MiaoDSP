#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = D:/ti/bios_6_73_01_01/packages;E:/ti_workspace_v8/miao/.config
override XDCROOT = D:/ti/xdctools_3_50_08_24_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = D:/ti/bios_6_73_01_01/packages;E:/ti_workspace_v8/miao/.config;D:/ti/xdctools_3_50_08_24_core/packages;..
HOSTOS = Windows
endif

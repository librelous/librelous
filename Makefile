#
# Quake3 Unix Makefile
#
# GNU Make required
#


# --Makefile variables--
MOUNT_DIR=./src
Q3A_DIR=/home/tma/.q3a/
MOD_DIR=tremulous

# --object list--
GOBJ = \
	$(GDIRNAME)/g_main.o \
	$(GDIRNAME)/bg_misc.o \
	$(GDIRNAME)/bg_pmove.o \
	$(GDIRNAME)/bg_slidemove.o \
	$(GDIRNAME)/g_mem.o \
	$(GDIRNAME)/q_math.o \
	$(GDIRNAME)/q_shared.o \
	$(GDIRNAME)/g_active.o \
	$(GDIRNAME)/g_arenas.o \
	$(GDIRNAME)/g_client.o \
	$(GDIRNAME)/g_cmds.o \
	$(GDIRNAME)/g_combat.o \
	$(GDIRNAME)/g_items.o \
	$(GDIRNAME)/g_buildable.o \
	$(GDIRNAME)/g_misc.o \
	$(GDIRNAME)/g_missile.o \
	$(GDIRNAME)/g_mover.o \
	$(GDIRNAME)/g_session.o \
	$(GDIRNAME)/g_spawn.o \
	$(GDIRNAME)/g_svcmds.o \
	$(GDIRNAME)/g_target.o \
	$(GDIRNAME)/g_team.o \
	$(GDIRNAME)/g_trigger.o \
	$(GDIRNAME)/g_utils.o \
	$(GDIRNAME)/g_weapon.o

CGOBJ = \
	$(CGDIRNAME)/cg_main.o \
	$(GDIRNAME)/bg_misc.o \
	$(GDIRNAME)/bg_pmove.o \
	$(GDIRNAME)/bg_slidemove.o \
	$(GDIRNAME)/q_math.o \
	$(GDIRNAME)/q_shared.o \
	$(CGDIRNAME)/cg_consolecmds.o \
	$(CGDIRNAME)/cg_buildable.o \
	$(CGDIRNAME)/cg_draw.o \
	$(CGDIRNAME)/cg_drawtools.o \
	$(CGDIRNAME)/cg_effects.o \
	$(CGDIRNAME)/cg_ents.o \
	$(CGDIRNAME)/cg_event.o \
	$(CGDIRNAME)/cg_info.o \
	$(CGDIRNAME)/cg_localents.o \
	$(CGDIRNAME)/cg_marks.o \
	$(CGDIRNAME)/cg_players.o \
	$(CGDIRNAME)/cg_playerstate.o \
	$(CGDIRNAME)/cg_predict.o \
	$(CGDIRNAME)/cg_scoreboard.o \
	$(CGDIRNAME)/cg_servercmds.o \
	$(CGDIRNAME)/cg_snapshot.o \
	$(CGDIRNAME)/cg_view.o \
	$(CGDIRNAME)/cg_weapons.o \
	$(CGDIRNAME)/cg_creep.o  \
	$(CGDIRNAME)/cg_mem.o \
	$(CGDIRNAME)/cg_mp3decoder.o \
	$(CGDIRNAME)/cg_scanner.o
  
UIOBJ = \
	$(UIDIRNAME)/ui_main.o \
	$(GDIRNAME)/bg_misc.o \
	$(GDIRNAME)/q_math.o \
	$(GDIRNAME)/q_shared.o \
	$(UIDIRNAME)/ui_addbots.o \
	$(UIDIRNAME)/ui_atoms.o \
	$(UIDIRNAME)/ui_cdkey.o \
	$(UIDIRNAME)/ui_cinematics.o \
	$(UIDIRNAME)/ui_confirm.o \
	$(UIDIRNAME)/ui_connect.o \
	$(UIDIRNAME)/ui_controls2.o \
	$(UIDIRNAME)/ui_credits.o \
	$(UIDIRNAME)/ui_demo2.o \
	$(UIDIRNAME)/ui_display.o \
	$(UIDIRNAME)/ui_gameinfo.o \
	$(UIDIRNAME)/ui_ingame.o \
	$(UIDIRNAME)/ui_loadconfig.o \
	$(UIDIRNAME)/ui_menu.o \
	$(UIDIRNAME)/ui_mfield.o \
	$(UIDIRNAME)/ui_mods.o \
	$(UIDIRNAME)/ui_network.o \
	$(UIDIRNAME)/ui_options.o \
	$(UIDIRNAME)/ui_playermodel.o \
	$(UIDIRNAME)/ui_players.o \
	$(UIDIRNAME)/ui_playersettings.o \
	$(UIDIRNAME)/ui_preferences.o \
	$(UIDIRNAME)/ui_qmenu.o \
	$(UIDIRNAME)/ui_removebots.o \
	$(UIDIRNAME)/ui_saveconfig.o \
	$(UIDIRNAME)/ui_serverinfo.o \
	$(UIDIRNAME)/ui_servers2.o \
	$(UIDIRNAME)/ui_setup.o \
	$(UIDIRNAME)/ui_sound.o \
	$(UIDIRNAME)/ui_sparena.o \
	$(UIDIRNAME)/ui_specifyserver.o \
	$(UIDIRNAME)/ui_splevel.o \
	$(UIDIRNAME)/ui_sppostgame.o \
	$(UIDIRNAME)/ui_spskill.o \
	$(UIDIRNAME)/ui_startserver.o \
	$(UIDIRNAME)/ui_team.o \
	$(UIDIRNAME)/ui_teamorders.o \
	$(UIDIRNAME)/ui_video.o \
	$(UIDIRNAME)/ui_dynamicmenu.o
  




  
# --You shouldn't have to touch anything below here--

# --general variables--
PLATFORM=$(shell uname|tr A-Z a-z)
PLATFORM_RELEASE=$(shell uname -r)

BD=debug$(ARCH)$(GLIBC)
BR=release$(ARCH)$(GLIBC)
BQ=qvm

GDIRNAME=game
CGDIRNAME=cgame
UIDIRNAME=q3_ui
GDIR=$(MOUNT_DIR)/$(GDIRNAME)
CGDIR=$(MOUNT_DIR)/$(CGDIRNAME)
UIDIR=$(MOUNT_DIR)/$(UIDIRNAME)


# --gcc config--
ifneq (,$(findstring libc6,$(shell if [ -e /lib/libc.so.6* ];then echo libc6;fi)))
GLIBC=-glibc
else
GLIBC=
endif #libc6 test


ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
RPMARCH=alpha
VENDOR=dec
else #default to i386
ARCH=i386
RPMARCH=i386
VENDOR=unknown
endif #alpha test

BASE_CFLAGS=-Dstricmp=strcasecmp -I/usr/X11R6/include -I/usr/include/glide -pipe
DEBUG_CFLAGS=$(BASE_CFLAGS) -g -pg
DEPEND_CFLAGS= -MM

ifeq ($(ARCH),axp)
CC=pgcc
RELEASE_CFLAGS=$(BASE_CFLAGS) -DNDEBUG -O6 -ffast-math -funroll-loops -fomit-frame-pointer -fexpensive-optimizations
else
NEWPGCC=/usr/local/gcc-2.95.2/bin/gcc
CC=$(shell if [ -f $(NEWPGCC) ]; then echo $(NEWPGCC); else echo gcc; fi )
RELEASE_CFLAGS=$(BASE_CFLAGS) -DNDEBUG -O6 -mcpu=pentiumpro -march=pentium -fomit-frame-pointer -pipe -ffast-math -malign-loops=2 -malign-jumps=2 -malign-functions=2 -fno-strict-aliasing -fstrength-reduce 
endif

LIBEXT=a

SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

ARFLAGS=ar rv
RANLIB=ranlib

THREAD_LDFLAGS=-lpthread
LDFLAGS=-ldl -lm

SED=sed



# --qvm building config--
LCC_CPP=q3cpp
LCC_CPP_FLAGS=-DQ3_VM
LCC_INCLUDES=-I$(CGDIR) -I$(GDIR) -I$(UIDIR)

LCC_RCC=q3rcc
LCC_RCC_FLAGS=-target=bytecode -g

Q3ASM=q3asm
Q3ASM_FLAGS=



# --main targets--
all: release debug qvm

release: $(BR)/cgame$(ARCH).$(SHLIBEXT) $(BR)/qagame$(ARCH).$(SHLIBEXT) $(BR)/ui$(ARCH).$(SHLIBEXT)

debug: $(BD)/cgame$(ARCH).$(SHLIBEXT) $(BD)/qagame$(ARCH).$(SHLIBEXT) $(BD)/ui$(ARCH).$(SHLIBEXT)

qvm: $(BQ)/cgame.qvm $(BQ)/qagame.qvm $(BQ)/ui.qvm

makedirs:
	@if [ ! -d $(B) ];then mkdir $(B);fi
	@if [ ! -d $(B)/$(GDIRNAME) ];then mkdir $(B)/$(GDIRNAME);fi
	@if [ ! -d $(B)/$(CGDIRNAME) ];then mkdir $(B)/$(CGDIRNAME);fi
	@if [ ! -d $(B)/$(UIDIRNAME) ];then mkdir $(B)/$(UIDIRNAME);fi



# --object lists for each build type--
GQVMOBJ = $(GOBJ:%.o=$(BQ)/%.asm)
GROBJ = $(GOBJ:%.o=$(BR)/%.o) $(BR)/$(GDIRNAME)/g_syscalls.o
GDOBJ = $(GOBJ:%.o=$(BD)/%.o) $(BD)/$(GDIRNAME)/g_syscalls.o

CGQVMOBJ = $(CGOBJ:%.o=$(BQ)/%.asm)
CGROBJ = $(CGOBJ:%.o=$(BR)/%.o) $(BR)/$(CGDIRNAME)/cg_syscalls.o
CGDOBJ = $(CGOBJ:%.o=$(BD)/%.o) $(BD)/$(CGDIRNAME)/cg_syscalls.o

UIQVMOBJ = $(UIOBJ:%.o=$(BQ)/%.asm)
UIROBJ = $(UIOBJ:%.o=$(BR)/%.o) $(BR)/$(UIDIRNAME)/ui_syscalls.o
UIDOBJ = $(UIOBJ:%.o=$(BD)/%.o) $(BD)/$(UIDIRNAME)/ui_syscalls.o



# --rules for the shared objects--
#release qagamei386.so
$(BR)/qagame$(ARCH).$(SHLIBEXT) : $(GROBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GROBJ)
  
#debug qagamei386.so
$(BD)/qagame$(ARCH).$(SHLIBEXT) : $(GDOBJ)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GDOBJ)
  
#qvm qagame.qvm
$(BQ)/qagame.qvm : $(GQVMOBJ) $(BQ)/$(GDIRNAME)/bg_lib.asm
	$(Q3ASM) $(Q3ASM_FLAGS) -o $@ $(GQVMOBJ) $(GDIR)/g_syscalls.asm $(BQ)/$(GDIRNAME)/bg_lib.asm
  
  
#release cgamei386.so
$(BR)/cgame$(ARCH).$(SHLIBEXT) : $(CGROBJ)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(CGROBJ)

#debug cgamei386.so
$(BD)/cgame$(ARCH).$(SHLIBEXT) : $(CGDOBJ)
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(CGDOBJ)

#qvm cgame.qvm
$(BQ)/cgame.qvm :	$(CGQVMOBJ) $(BQ)/$(GDIRNAME)/bg_lib.asm
	$(Q3ASM) $(Q3ASM_FLAGS) -o $@ $(CGQVMOBJ) $(CGDIR)/cg_syscalls.asm $(BQ)/$(GDIRNAME)/bg_lib.asm

  
#release uii386.so
$(BR)/ui$(ARCH).$(SHLIBEXT) : $(UIROBJ) 
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(UIROBJ)

#debug cgamei386.so
$(BD)/ui$(ARCH).$(SHLIBEXT) : $(UIDOBJ) 
	$(CC) $(DEBUG_CFLAGS) $(SHLIBLDFLAGS) -o $@ $(UIDOBJ)
  
#qvm ui.qvm
$(BQ)/ui.qvm: $(UIQVMOBJ) $(BQ)/$(GDIRNAME)/bg_lib.asm
	$(Q3ASM) $(Q3ASM_FLAGS) -o $@ $(UIQVMOBJ) $(UIDIR)/ui_syscalls.asm $(BQ)/$(GDIRNAME)/bg_lib.asm



# --rules for the objects--
#release g_*.o
$(BR)/$(GDIRNAME)/%.o: $(GDIR)/%.c 
	$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
#debug g_*.o
$(BD)/$(GDIRNAME)/%.o: $(GDIR)/%.c 
	$(CC) $(DEBUG_CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
#qvm g_*.asm
$(BQ)/$(GDIRNAME)/%.asm: $(GDIR)/%.c 
	$(LCC_CPP) $(LCC_CPP_FLAGS) $(LCC_INCLUDES) $< $(BQ)/$(GDIRNAME)/$*.i
	$(LCC_RCC) $(LCC_RCC_FLAGS) $(BQ)/$(GDIRNAME)/$*.i $@
	rm $(BQ)/$(GDIRNAME)/$*.i


#release cg_*.o
$(BR)/$(CGDIRNAME)/%.o: $(CGDIR)/%.c 
	$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
#debug cg_*.o
$(BD)/$(CGDIRNAME)/%.o: $(CGDIR)/%.c 
	$(CC) $(DEBUG_CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<

#qvm cg_*.asm
$(BQ)/$(CGDIRNAME)/%.asm: $(CGDIR)/%.c 
	$(LCC_CPP) $(LCC_CPP_FLAGS) $(LCC_INCLUDES) $< $(BQ)/$(CGDIRNAME)/$*.i
	$(LCC_RCC) $(LCC_RCC_FLAGS) $(BQ)/$(CGDIRNAME)/$*.i $@
	rm $(BQ)/$(CGDIRNAME)/$*.i


#release ui_*.o
$(BR)/$(UIDIRNAME)/%.o: $(UIDIR)/%.c 
	$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
#debug ui_*.o
$(BD)/$(UIDIRNAME)/%.o: $(UIDIR)/%.c 
	$(CC) $(DEBUG_CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
  
#qvm ui_*.asm
$(BQ)/$(UIDIRNAME)/%.asm: $(UIDIR)/%.c 
	$(LCC_CPP) $(LCC_CPP_FLAGS) $(LCC_INCLUDES) $< $(BQ)/$(UIDIRNAME)/$*.i
	$(LCC_RCC) $(LCC_RCC_FLAGS) $(BQ)/$(UIDIRNAME)/$*.i $@
	rm $(BQ)/$(UIDIRNAME)/$*.i



# --cleaning rules--
clean:clean-debug clean-release clean-qvm

clean-debug:
	rm -f $(BD)/$(GDIRNAME)/*.o
	rm -f $(BD)/$(CGDIRNAME)/*.o
	rm -f $(BD)/$(UIDIRNAME)/*.o

clean-release:
	rm -f $(BR)/$(GDIRNAME)/*.o
	rm -f $(BR)/$(CGDIRNAME)/*.o
	rm -f $(BR)/$(UIDIRNAME)/*.o

clean-qvm:
	rm -f $(BQ)/$(GDIRNAME)/*.asm
	rm -f $(BQ)/$(CGDIRNAME)/*.asm
	rm -f $(BQ)/$(UIDIRNAME)/*.asm



# --installing rules--
install-release:release
	mkdir -p $(Q3A_DIR)/$(MOD_DIR)
	cp $(BR)/*.so $(Q3A_DIR)/$(MOD_DIR)

install-debug:debug
	mkdir -p $(Q3A_DIR)/$(MOD_DIR)
	cp $(BD)/*.so $(Q3A_DIR)/$(MOD_DIR)

install-qvm:qvm
	mkdir -p $(Q3A_DIR)/$(MOD_DIR)/vm
	cp $(BQ)/*.qvm $(Q3A_DIR)/$(MOD_DIR)/vm


# --dependency rules--
DEPEND_FILE=depend

depend:
	echo > $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(GDIR)/*.c | $(SED) -e 's/^\(.*\.o\)/$(BR)\/$(GDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(GDIR)/*.c | $(SED) -e 's/^\(.*\.o\)/$(BD)\/$(GDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(GDIR)/*.c | $(SED) -e 's/\.o/.asm/g' -e 's/^\(.*\.asm\)/$(BQ)\/$(GDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(CGDIR)/*.c | $(SED) -e 's/^\(.*\.o\)/$(BR)\/$(CGDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(CGDIR)/*.c | $(SED) -e 's/^\(.*\.o\)/$(BD)\/$(CGDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(CGDIR)/*.c | $(SED) -e 's/\.o/.asm/g' -e 's/^\(.*\.asm\)/$(BQ)\/$(CGDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(UIDIR)/*.c | $(SED) -e 's/^\(.*\.o\)/$(BR)\/$(UIDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(UIDIR)/*.c | $(SED) -e 's/^\(.*\.o\)/$(BD)\/$(UIDIRNAME)\/\1/g' >> $(DEPEND_FILE)
	$(CC) $(DEPEND_CFLAGS) $(LCC_INCLUDES) $(UIDIR)/*.c | $(SED) -e 's/\.o/.asm/g' -e 's/^\(.*\.asm\)/$(BQ)\/$(UIDIRNAME)\/\1/g' >> $(DEPEND_FILE)
        
include $(DEPEND_FILE) 


#source file search paths
VPATH = ./src ../raven-set ../../code_ext/boost/boost1_78

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \
	-I../../code_ext/boost/boost1_78

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static -lComctl32

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_test1 = \
	$(ODIR)/test1.o \
	$(ODIR)/cSolutionSpaceExplorer.o \
	$(ODIR)/cRunWatch.o \

OBJ_test2 = \
	$(ODIR)/test2.o \
	$(ODIR)/cSolutionSpaceExplorer.o \
	$(ODIR)/cRunWatch.o \

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ $< \
	-c -o $@  $(INCS) $(CCflags)

test1: $(OBJ_test1)
	g++ $(OPT) \
	-o bin/test1.exe $^  \
	$(static_link_flags) $(LIBS)

test2: $(OBJ_test2)
	g++ $(OPT) \
	-o bin/test2.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o


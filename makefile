# Root variables
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs) -lMinuit -lRooFit -lRooStats -lRooFitCore -lFoam -lHistFactory -lXMLParser 
#-lRooFit -lRooStats 
#-lg2c  
#FORTRANLIBS  := -L../lib/ -L/cern/pro/lib 
#-lkernlib

# Programs
CXX          = g++
CXXFLAGS     = -g -Wall -fPIC -Wno-deprecated 
LDFLAGS      = -g 
SOFLAGS      = -shared 

# Direct to make the directories with the sources:
VPATH  = ./lib / ./bin ./src ./aux

# Assign or Add variables
CXXFLAGS    += $(ROOTCFLAGS)
CXXFLAGS    += $(INCDIR)
LIBS        += $(ROOTLIBS)

CXXOBJS      = $(CXXSRCS:.cc=.o)
GARBAGE      = $(CXXOBJS) libana.so *~ lib/*.so lib/*.o ./*/*.o


############################################################################
# Build main.c compiling only the first prereq: ($<) means main.c
############################################################################
makespace : makespace.o
makespace_edge : makespace_edge.o
toy : toy.o
cls : cls.o likelihoodtree.o
q0 : q0.o likelihoodtree.o
qmu : qmu.o
limit : limit.o
############################################################################
# General rules. The first two need not be specified due to implicit rules, 
# but redefined for the "echo"
############################################################################
%.o : %.cc %.hh
	@echo "compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< 

%.o : %.cc 
	@echo "compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< 

%    : %.o
	@echo "compiling and linking $@"
	@$(CXX) $(LDFLAGS) $^ \
	        $(FORTRANLIBS) $(ROOTLIBS)  -o $@
	@if [ `ls | grep "\.so"$ | wc -l` != 0 ]; then mv *.so lib/; fi
	@if [ `ls | grep "\.o"$  | wc -l` != 0 ]; then mv *.o  lib/; fi
	@mv $@ bin/

############################################################################
# Build libana.so compiling all prerequisites: ($^) means CXXOBJS 
# ($@) means the target libana.so
############################################################################
libana.so : $(CXXOBJS) 
	@echo "linking libana.so"
	@$(CXX) $(SOFLAGS) $(LDFLAGS) $^ -o $@


############################################################################
# Phony rules (no prerequisites)
############################################################################

.PHONY: clean cln print xemacs backup clnPM

clean :
	@rm -f $(GARBAGE)

cln :
	@rm -f *~ *.o */*~

clnPM :
	@rm -f usePM pairManager.o SingleElectron.o */*~

print :
	@echo compiler  : $(CXX)
	@echo c++ srcs  : $(CXXSRCS)
	@echo c++ objs  : $(CXXOBJS)
	@echo c++ flags : $(CXXFLAGS)
	@echo libs      : $(LIBS)
	@echo so flags  : $(SOFLAGS)
	@echo rootlibs  : $(ROOTLIBS)



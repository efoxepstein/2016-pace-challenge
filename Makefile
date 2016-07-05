EXECSOURCES := src/tw-heuristic.cpp
SOURCES := src/minimum_degree_heuristic.cpp

DEBUG ?= 0

###################################################

SRCDIR := src
OBJDIR := obj
BINDIR := bin

OBJECTS := $(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))
EXECOBJECTS := $(addprefix $(OBJDIR)/,$(notdir $(EXECSOURCES:.cpp=.o)))
BINARIES := $(addprefix $(BINDIR)/,$(notdir $(EXECSOURCES:.cpp=)))

CXX        = $(shell which clang++ || which g++)

CXXFLAGS   = -std=c++11 -Isrc -Werror -Weverything
CXXFLAGS  += -Wno-c++98-compat -Wno-global-constructors -Wno-padded
CXXFLAGS  += -Wno-disabled-macro-expansion

LDFLAGS    =

ifeq ($(DEBUG), 1)
  CXXFLAGS += -DDEBUG -O0 -fno-omit-frame-pointer -Werror -g
else
  CXXFLAGS += -DNDEBUG -O3 -fno-exceptions -fomit-frame-pointer -march=native
endif

CMD = $(CXX) $(CXXFLAGS) -MMD -MP

all: $(OBJECTS) $(EXECOBJECTS) $(BINARIES) symlink
.PHONY: all force

CLEANFLAGS = $(CMD) $(LDFLAGS)
cleanfile: force
	@echo '$(CLEANFLAGS)' | cmp -s - $@ || echo '$(CLEANFLAGS)' > cleanfile

$(BINARIES) : $(BINDIR)/% : $(OBJDIR)/%.o $(OBJECTS)
	$(CMD) -o $@ $^ $(LDFLAGS) 

$(OBJECTS) : | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR) $(BINDIR)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp cleanfile
	$(CMD) -o $@ -c $<

symlink: bin/tw-heuristic
	@rm -f tw-heuristic
	@ln -s bin/tw-heuristic tw-heuristic

clean:
	rm -f $(BINARIES) *.d $(OBJDIR)/*.d *.o $(OBJDIR)/*.o

test: bin/tw-heuristic bin/td-validate
	bin/test_all

kill:
	@kill -9 `pidof tw-heuristic`

-include $(wildcard $(OBJDIR)/*.d)


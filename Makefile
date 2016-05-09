EXECS := bin/td-validate bin/tw-heuristic
EXECSOURCES := src/td-validate.cpp src/tw-heuristic.cpp
SOURCES := src/minimum_degree_heuristic.cpp

########## PROBABLY NO NEED TO CHANGE BELOW HERE ########## 

OBJECTS = $(SOURCES:.cpp=.o)

CXX        = $(shell which clang++ || which g++)
CXXFLAGS   = -std=c++11 -Isrc -g
LDFLAGS    = -lprofiler
DEBUGFLAGS = -g -fno-omit-frame-pointer -Werror
WARNFLAGS  = -Wall -Wextra -Wpedantic

DEBUG ?= 1

CXXFLAGS += $(WARNFLAGS)

ifeq ($(DEBUG), 1)
  CXXFLAGS += $(DEBUGFLAGS) -O0
else
  CXXFLAGS += -DNDEBUG -O3 -fno-exceptions -fomit-frame-pointer -march=native
endif

CMD = $(CXX) $(CXXFLAGS) -MMD -MP

all: $(OBJECTS) $(EXECSOURCES:.cpp=.o) $(EXECS)
.PHONY: all force

CLEANFLAGS = $(CMD) $(LDFLAGS)
cleanfile: force
	@echo '$(CLEANFLAGS)' | cmp -s - $@ || echo '$(CLEANFLAGS)' > cleanfile

$(EXECS) : bin/% : src/%.o $(OBJECTS)
	$(CMD) -o $@ $^ $(LDFLAGS) 

src/%.o : src/%.cpp cleanfile
	$(CMD) -o $@ -c $<

clean:
	rm -f $(EXECS)
	rm -f *.d bin/*.d src/*.d
	rm -f *.o bin/*.o src/*.o
	rm -f *.gcno 
	rm -f *.gcda

-include $(wildcard *.d)
-include $(wildcard bin/*.d)
-include $(wildcard src/*.d)


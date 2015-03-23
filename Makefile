all:	trax trax-httpd

CXXFLAGS = -Wall
CXXFLAGS += -std=c++11
CXXFLAGS += -g

SRCS = trax.cc move.cc trace.cc validation.cc
OBJS = $(SRCS:%.cc=%.o)

.SUFFIXES: .cc

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

trax-httpd: trax-httpd.cc
	$(CXX) $(CXXFLAGS) -o trax-httpd $(LDFLAGS) trax-httpd.cc

trax:	$(OBJS)
	$(CXX) $(CXXFLAGS) -o trax $(OBJS) $(LDFLAGS)

all:	trax

trax.o: solver.hpp board.hpp board_osana.hpp test_board.hpp

clean:
	-rm -rf *.o *~ core trax trax-httpd

clean_record:
	-rm -rf *.trx

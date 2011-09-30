
CXX = g++
CFLAGS = -g -Wall -O2 -W -std=c++0x `pkg-config opencv --cflags`
LIBS = `pkg-config opencv --libs`
INCPATH = -I.
EXE = model

OBJECTS = shape.o \
          transform.o \
          object.o \
          model.o \
          surface.o \
          camera.o \
          main.o \

HEADERS = Makefile \
          surface.h \
          object.h \
          model.h \
          transform.h \
          shape.h \
          lexer.h \
          camera.h \
          matrix.tpp \
          queue.tpp \
          Vector.tpp \

all: $(EXE)
$(EXE): $(OBJECTS) $(HEADERS)
	$(CXX) -o $(EXE) $(OBJECTS) $(LIBS)

debug: clean
	$(MAKE) DEF=-DDEBUG

$(OBJECTS) : %.o : %.cpp $(HEADERS)
	$(CXX) -c $(INCPATH) $(DEF) $(CFLAGS) $<

clean:
	rm -f *.o $(EXE) output.pgm

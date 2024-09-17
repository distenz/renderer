SYSCONF_LINK = clang++
CPPFLAGS     = -ggdb -std=c++17
LDFLAGS      =
LIBS         =

DESTDIR = ./build/
TARGET  = main
ARTIFACT = artifact.tga

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard src/*.cpp) $(wildcard include/*.cpp) $(wildcard example/*.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f $(ARTIFACT)


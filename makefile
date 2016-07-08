CXX = gcc -std=c99
LDFLAGS = `pkg-config --libs SDL_image` `pkg-config --libs SDL_ttf` `pkg-config --libs SDL_mixer`
CXXFLAGS = `pkg-config --cflags SDL_image` `pkg-config --cflags SDL_ttf` `pkg-config --cflags SDL_mixer`
TARGET = game

SRCS = main.c mainmenu.c audioengine.c about.c highscores.c maingame.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET) 

%.o : %.c
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@


clean:
	rm -rf *.o game *.d

-include $(DEPS)

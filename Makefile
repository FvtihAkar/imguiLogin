EXE = main
IMGUI_DIR = ./imgui
SOURCES = main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)
OS := $(shell echo $(SHELL) | grep -i -o mingw)

# Libraries for Linux
LINUX_GL_LIBS = -lGL -lcurl
LINUX_GLFW_LIBS = `pkg-config --static --libs glfw3`
LINUX_GLFW_CFLAGS = `pkg-config --cflags glfw3`

# Libraries for Windows
WIN_GL_LIBS = -lopengl32 -lglfw3
WIN_CFLAGS = -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends

CXXFLAGS = -std=c++17 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -Wall -Wformat
LIBS =

ifeq ($(UNAME_S), Linux) # Linux
    ECHO_MESSAGE = "Linux"
    LIBS += $(LINUX_GL_LIBS) $(LINUX_GLFW_LIBS)
    CXXFLAGS += $(LINUX_GLFW_CFLAGS)
    CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), mingw) # Windows with MinGW
    ECHO_MESSAGE = "Windows (MinGW)"
    LIBS += $(WIN_GL_LIBS)
    CXXFLAGS += $(WIN_CFLAGS)
    CFLAGS = $(CXXFLAGS)
endif

# Rule for compiling .cpp files in the build directory
%.o: build/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule for compiling .cpp files from the imgui directory
%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Main build target
all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)
	@$(MAKE) clean_objs

# Link the object files into the final executable
$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

# Clean the build files
clean:
	rm -f $(EXE)

# Clean object files
clean_objs:
	rm -f $(OBJS)

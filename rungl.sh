#!/usr/bin/bash
g++ -I includes  src/*.cpp src/glad.c -o glout -Wall -lGL -lGLU -lglut -lGLEW -lglfw -lX11 -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lm -lassimp
./glout
rm glout

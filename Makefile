all:
	g++ -o trabalhocg *.cpp -lGL -lGLU -lglut
clean:
	rm -f trabalhocg *.o
run:
	./trabalhocg

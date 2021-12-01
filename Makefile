
# CUDA includes and libraries
F1= -L/usr/local/cuda-10.1/lib64
#F2= -I/usr/local/cuda-9.2/targets/x86_64-linux/include -lcuda -lcudart
F2= -I/usr/local/cuda-10.1/targets/x86_64-linux/include -lcuda -lcudart

# SDL shtuff (for sound processing)
F3= -I/usr/local/include -L/usr/local/lib -lSDL2
F4= -std=c++11

# animation libraries:
F5= -lglut -lGL

all: musVis

musVis: interface.o gpu_main.o animate.o audio.o
	g++ -o musVis interface.o gpu_main.o animate.o audio.o $(F1) $(F2) $(F3) $(F4) $(F5)

# do we really need all these flags to compile interface??!!
interface.o: interface.cpp interface.h animate.h animate.cu audio.c audio.h
	g++ -w -c interface.cpp $(F1) $(F2) $(F3) $(F4)

gpu_main.o: gpu_main.cu gpu_main.h
	nvcc -w -c gpu_main.cu

animate.o: animate.cu animate.h gpu_main.h
	nvcc -w -c animate.cu
#	nvcc -w -c animate.cu $(ANIMLIBS)

audio.o: audio.c audio.h
	g++ -w -c audio.c $(F2)

clean:
	rm interface.o;
	rm gpu_main.o;
	rm animate.o;
	rm audio.o;
	rm musVis;

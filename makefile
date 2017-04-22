compile: 
	mpic++ -std=c++14 -Wall -pedantic -fopenmp -O3 src/*.cpp -o bw.out

valrun:
	valgrind ./bw.out

clean:
	rm -f ./bw.out

run:
	mpirun bw.out -f data/graph12_5.txt -a 6 -t 2

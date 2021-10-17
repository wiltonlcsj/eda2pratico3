run:
	g++ manipula.c -o program
	./program

run-test:
	rm -f dados
	rm -f arvore
	g++ manipula.c -o program
	./program < ./in/1.in > 1-p.out
	cmp ./out/1.out 1-p.out
	rm 1-p.out

clear:
	rm -f dados
	rm -f arvore
	rm -f program

make run-clear:
	make clear
	make run
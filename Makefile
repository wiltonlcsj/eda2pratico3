run:
	gcc manipula.c -o program -lm
	./program

run-test:
	rm -f dados
	rm -f arvore
	gcc manipula.c -o program -lm
	./program < ./in/1.in > 1-p.out
	cmp ./out/1.out 1-p.out
	rm 1-p.out

run-test-2:
	rm -f dados
	rm -f arvore
	gcc manipula.c -o program -lm
	./program < ./in/2.in > 2-p.out

run-test-3:
	rm -f dados
	rm -f arvore
	gcc manipula.c -o program -lm
	./program < ./in/3.in > 3-p.out

clear:
	rm -f dados
	rm -f arvore
	rm -f program

make run-clear:
	make clear
	make run
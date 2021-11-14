run:
	gcc manipula.c -o program
	./program

run-test:
	rm -f dados.dat
	rm -f arvore.dat
	rm -f lista.dat
	gcc manipula.c -o program
	./program < ./in/1.in > 1-p.out
	#cmp ./out/1.out 1-p.out
	#rm 1-p.out

run-test-2:
	rm -f dados.dat
	rm -f arvore.dat
	rm -f lista.dat
	gcc manipula.c -o program
	./program < ./in/2.in > 2-p.out
	cmp ./out/2.out 2-p.out
	rm 2-p.out

clear:
	rm -f dados.dat
	rm -f arvore.dat
	rm -f lista.dat
	rm -f program

make run-clear:
	make clear
	make run
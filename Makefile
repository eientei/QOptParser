all:
	cd project && qmake && make 

clean:
	cd project && qmake && make clean

distclean:
	cd project && qmake && make distclean
	cd project && qmake CONFIG+=unittest CONFIG+=debug && make distclean

debug:
	cd project && qmake CONFIG+=unittest CONFIG+=debug && make




OUT = ./out
UTILITY = ./utility
SEMAFORI = $(UTILITY)/semafori/semafori.c
OTTIENI_DATI = $(UTILITY)/ottieni_dati/ottieni_dati.c
FUNZIONI_CONDIVISE = $(UTILITY)/funzioni_condivise/funzioni_condivise.c
MEMORIA_CONDIVISA = $(UTILITY)/memoria_condivisa/memoria_condivisa.c
MESSAGGI  = $(UTILITY)/messaggi/messaggi.c

$(shell mkdir -p $(OUT))


all: nave porto master


nave: nave.c ./utility/semafori/semafori.c
	gcc -std=c89  -Wpedantic -g -o $(OUT)/nave nave.c $(SEMAFORI) $(OTTIENI_DATI) $(FUNZIONI_CONDIVISE) $(MESSAGGI) -lm -lpthread

porto: porto.c ./utility/semafori/semafori.c
	gcc -std=c89  -Wpedantic -g -o  $(OUT)/porto porto.c  $(SEMAFORI) $(OTTIENI_DATI) $(FUNZIONI_CONDIVISE) $(MEMORIA_CONDIVISA) $(MESSAGGI) -lm -lpthread

master: master.c ./utility/semafori/semafori.c
	gcc -std=c89 -Wpedantic -g -o  $(OUT)/master master.c  $(SEMAFORI) $(OTTIENI_DATI) $(MEMORIA_CONDIVISA) $(FUNZIONI_CONDIVISE) $(MESSAGGI) -lm -lpthread
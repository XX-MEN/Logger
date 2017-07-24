/*

Program Loger.c loguje swoje działanie do bufora oraz pliku o nazwie:

"nazwa_programu_RRRRMMDD_GG_MM.log"

Sprawdza czy plik o takiej nazwie istnieje, jeżeli nie:
- generuje LOG_ERROR
- zapisuje LOG_ERROR w buforze
- tworzy nowy plik do logowania wg schematu j.w.
- zapisuje bufor do pliku w przypadku zapełnienia buforu.
- dodatkowo log zawiera informacje o tym z jakiej funkcji został wywołany.

Jeżeli program zostanie uruchomiony wielokrotnie w przeciągu minuty MM
- LOG_ERROR nie zostanie wygenerowany.
- logi będą dopisywane w istniejącym pliku.

Użycie makr w funkcji main:
	LOG_INFO;
	LOG_LOW("message");
	LOG_MEDIUM("message");
	LOG_HIGH("message");
	LOG_ERROR("message");
	LOG_END; //zrzuca bufor nieoprozniony bez komunikatu.

spowoduje wygenerowanie takich logów:

▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ Log programu ..\Loger.c ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
Jul 24 2017 | 12:13:30 | ..\Loger.c:  128 | LOW    | message (funkcja main)
Jul 24 2017 | 12:13:30 | ..\Loger.c:  129 | MEDIUM | message (funkcja main)
Jul 24 2017 | 12:13:30 | ..\Loger.c:  130 | HIGH   | message (funkcja main)
Jul 24 2017 | 12:13:30 | ..\Loger.c:  131 | ERROR  | message (funkcja main)
//LOG_END; zrzuca bufor nieoprozniony bez komunikatu.

W kodzie zanjduje się również przykład zastosowania
oparty na przykładowej funkcji "losuj_liczbe"

*
* Loger.c
*
* Copyright 2017 Krzysztof Chwalisz <krzysztof.chwalisz@wp.pl>
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFFER_LEN 300
#define nazwaloga_LEN 80

int bufavaliable = BUFFER_LEN;
char buf[BUFFER_LEN];
char linebuf[BUFFER_LEN];
char nazwaloga[nazwaloga_LEN];
FILE* file;

#if __STDC_VERSION__ >= 199901L
#define __LOG_LINE(log_level,message) "%s | %s | %s: %4d | %-6s | %s (funkcja %s)\n", __DATE__, __TIME__, __FILE__, __LINE__, log_level,message,__func__
#else
#define __LOG_LINE(log_level,message) "%s | %s | %s: %4d | %-6s | %s\n", __DATE__, __TIME__, __FILE__, __LINE__, log_level,message
#endif

#define BUFOROWANIE(log_level,message) \
		{	sprintf(linebuf, __LOG_LINE(log_level,message) );\
			if(bufavaliable > strlen(linebuf)) \
				{ 	sprintf(buf, "%s%s",buf,linebuf,__LOG_LINE(log_level,message) );\
					bufavaliable-=strlen(linebuf);\
				} \
			else { 	fprintf(file ,buf); \
					bufavaliable=BUFFER_LEN;\
					buf[0]='\0'; \
					sprintf(buf, __LOG_LINE(log_level,message) );\
					bufavaliable-=strlen(linebuf);\
				 }\
		}
				
#define LOG_LOW(message) BUFOROWANIE("LOW",message)
#define LOG_MEDIUM(message) BUFOROWANIE("MEDIUM",message)
#define LOG_HIGH(message) BUFOROWANIE("HIGH",message)
#define LOG_ERROR(message) BUFOROWANIE("ERROR",message)
#define LOG_INFO fprintf(file ,"▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ Log programu %s ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓\n", __FILE__);
#define LOG_END \
			{	fprintf(file ,buf);\
				buf[0]='\0'; \
			}
								
int losuj_liczbe(int min, int max)
{
    LOG_HIGH("poczatek");
	int tmp;
    if (max>=min)
        max=max-min+1;
    else
		{
			tmp= min - max;	
			min= max;
			max= tmp;
		}
	LOG_HIGH("koniec");	
    return max?(rand()%max+min):min;
}

int fileExists (const char* fileName)
{
	
	FILE *plik;
	LOG_HIGH("poczatek");
    plik = fopen(fileName, "r");  
    if ( plik )
    {
		fclose(plik);
		LOG_MEDIUM("Plik istnieje");
		LOG_HIGH("koniec");
        return 1;
    }
    LOG_ERROR("plik nie istnieje");
	LOG_HIGH("koniec");
    return 0;
}

int main(int argc, char *argv[])
{
	int i;												// zmienne pomocnicze
	char dataiczas[32];
    time_t timer;
    struct tm* tm_info;
		
	time(&timer);										//generowanie nazwy loga
    tm_info = localtime(&timer);
	strftime(dataiczas, 32, "_%Y%m%d_%H-%M", tm_info);
	strcpy(nazwaloga,__FILE__);
	strcat(nazwaloga,dataiczas);
	strcat(nazwaloga,".log");
	
	if (fileExists(nazwaloga))  						// test obecności "aktywnego" loga
		{
		file= fopen(nazwaloga,"a");
		printf("\n Operacje beda logowane do istniejacego pliku");
		LOG_LOW("Plik do logowania istnieje");
		LOG_MEDIUM("otwarcie pliku do Logowania");
		}
	else 
		{
		file = fopen(nazwaloga,"a");
		printf("\n Operacje programu beda logowane do nowego pliku: %s\n", nazwaloga);
		LOG_INFO;
		LOG_ERROR("Brak pliku, utworzono nowy plik do logowania");
		}

	LOG_MEDIUM("________ program gotowy do dzialania");
	LOG_LOW(">>>>>>> zakonczono blok programu");

	printf("\n Program teraz cos sobie wylosuje: \n");		//dowolne działanie programu
	for (i=0;i<=3; i++) printf("Wylosowano liczby : %d\n",losuj_liczbe(1, 3000));	
	printf("\n Program sie zakonczy\n");

	LOG_LOW("_______ program zostal zamkniety");
	LOG_HIGH("********** koniec LOGa");
	LOG_END;

	printf("\nLogi z dzialania programu sa dostepne w pliku: %s\n",nazwaloga);
	fclose(file);
    return 0;
}


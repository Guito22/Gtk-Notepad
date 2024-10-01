#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>
#include <hunspell/hunspell.h>

FILE *contenido,*guardado;
Hunhandle *hunspell;

GtkWindow *window;
GtkTextView *texto[4]; //pags
GtkTextBuffer *buffer[4]; //textos
GtkScrolledWindow *ventana_scrolled;

GtkPopover *popover[2], *m_corr; //menu y menu correcion

GtkButton *boton[2];

GtkTextIter inicio,fin; //IMPORTANTE PARA CAMBIAR PALABRA


int cont=1; //contador de paginas activas
int i,pos_pag[4]={0,1,2,3},pos_btns[2]={0,1};

char lineas_txt[4][1000][100];
int num_lineas[4]={0,0,0,0};

gboolean encriptado = FALSE;






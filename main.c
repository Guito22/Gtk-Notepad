#include "globals.h" //variables globales
#include "subrayado.h" //funcion de subrayado
#include "encriptar.h" //todo lo relacionado al proceso de encriptacion
#include "archivo_txt.h" //todo lo relacionado a guardar info en txt
#include "estilos.h" //estilos css de los widgets


//funcion que detecta y maneja tab, flechas en paginas
gboolean teclas (GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data){
	int *p = (int *)user_data;
	int n=*p;
	
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer[n], &iter, gtk_text_buffer_get_insert(buffer[n]));
	if (keyval == GDK_KEY_Tab) {
        gtk_text_buffer_insert(buffer[n], &iter, "     ", 5);  // Inserta 5 espacios simulando tabulador
        return TRUE;  
    }
    //flechas para bajar de hoja
    if(keycode==39 || keycode==40){

        if(gtk_text_iter_is_end(&iter) && n+1<cont)
        	gtk_widget_grab_focus(texto[n+1]);
	}
	//flechas para subir de hoja
    if(keycode==38 || keycode==37){

        if(gtk_text_iter_is_start(&iter) && n>0)
        	gtk_widget_grab_focus(texto[n-1]);
	}

	scroll(n);
    return FALSE;  // Continue processing the event
}


//funcion que se encarga de crear los estilos del widget deseado con las propiedades css
void crear_css(GtkWidget *objeto, char *estilo){
	GtkCssProvider *provider_objeto;
	provider_objeto = gtk_css_provider_new();
	gtk_css_provider_load_from_data(provider_objeto,estilo, -1);	
	
	GtkStyleContext *style_context_objeto = gtk_widget_get_style_context(objeto);
	//GTK_STYLE_PROVIDER_PRIORITY_APPLICATION PERMITE QUE SE APLIQUE EL ESTILO AL MOMENTO
	gtk_style_context_add_provider(style_context_objeto,provider_objeto,GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

//funcion para hacer funcionar ok de ventana de dialogo
void ayuda_salida(GtkDialog *dialogo,int id,gpointer user_data){
	gtk_window_close(dialogo);
}



//funcion para pasar a mayuscula despues de un . si no hay nada despues del punto
int may_pun(char *cadena){
	for(int k=0;k<strlen(cadena);++k){
		if(cadena[k]=='.' && cadena[k+1]==' ' && islower(cadena[k+2])){
			cadena[k+2]=toupper(cadena[k+2]);
			return 1;
		}
			
	}
	return 0;
}


//funcion que cuenta cuantas lineas visibles hay en un text_view
int cont_lineas(int p){
	GtkTextIter iter;
	int n=0;
	gboolean espacio;
	
	gtk_text_buffer_get_start_iter(buffer[p],&iter);
	do{
		if(gtk_text_iter_get_char(&iter)=='\n' && n==22){
			n=23;
			break;
		}
		if(gtk_text_view_starts_display_line(texto[p],&iter)){
			++n;
			if(n==23)
				break;
			
		}
		espacio = gtk_text_view_forward_display_line(texto[p],&iter);
	}
	while(espacio);
	
	return n;
	
}

//funcion que capta texto que no cabe en la pagina actual y pasa a las siguientes
int extra(int p){
	GtkTextIter aux,end,start_sig;
	gboolean status;
	int n=0;
	gchar *texto_extra;
	gtk_text_buffer_get_start_iter(buffer[p],&aux);
	//mismo proceso de cuando se cuentan lineas pero es para conservar el iterador del fin de hoja
	do{
		if(gtk_text_view_starts_display_line(texto[p],&aux)){
			++n;
			if(n==23)
				break;	
			
		}
		
		status = gtk_text_view_forward_display_line(texto[p],&aux);
	} while(status);	
	
	gtk_text_buffer_get_end_iter(buffer[p],&end);
	texto_extra = gtk_text_buffer_get_text(buffer[p],&aux,&end,-1);
	//se determina si hay texto extra
	if(strlen(texto_extra)>0 && p<3){
		//se hace visible la hoja de ser necesario
		crear_css(texto[p+1],e_hoja);
	 	gtk_widget_set_size_request(texto[p+1],600,600);
		
		if(p==cont-1)
			++cont;
			
		gtk_text_buffer_get_start_iter(buffer[p+1],&start_sig);
		
		if(texto_extra[0]!='\n' || (texto_extra[0]=='\n' && p<cont-1) ){
	        gtk_text_buffer_insert(buffer[p+1],&start_sig,texto_extra,-1);
		}

		gtk_text_buffer_delete(buffer[p],&aux,&end); //se borra el sobrante
				
		GtkTextIter iter;

   		gtk_text_buffer_get_iter_at_mark(buffer[p], &iter, gtk_text_buffer_get_insert(buffer[p]));
   		//se determina si es necesario cambiar foco de hoja
   		if(gtk_text_iter_is_end(&iter)){

			gtk_widget_grab_focus(texto[p+1]);
		   }
			
		
		g_free(texto_extra);
		return 1;
		
	}
	
	g_free(texto_extra);
	return 0;
	


}

//funciona que se encarga de posicionar el scroll en funcion del cursor
void scroll(int n){
	GtkTextIter iter;
	GdkRectangle location; //para mov de scroll
	gtk_text_buffer_get_iter_at_mark(buffer[n], &iter, gtk_text_buffer_get_insert(buffer[n]));

	// posicion del cursor en el text_view
	gtk_text_view_get_iter_location(texto[n], &iter, &location);
	
	GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(ventana_scrolled);

// Calcula la nueva posicion del viewport.
	gdouble new_value = location.y - gtk_adjustment_get_page_size(vadjustment) / 2;
	new_value = CLAMP(new_value, gtk_adjustment_get_lower(vadjustment), gtk_adjustment_get_upper(vadjustment) - gtk_adjustment_get_page_size(vadjustment));
	
//  cambia la posicion del scroll
	gtk_adjustment_set_value(vadjustment, new_value+((n)*560));	
}

//funcion que maneja cualquier cambio de los buffers
void cambio_buffer(gpointer user_data){
	//para evitar cualquier comportamiento si esta encriptado
	if(encriptado)
		return;
	//para conseguir que numero de hoja estamos usando
    int *p = (int *)user_data;
    int n = *p;
	GtkTextIter start,end;
	
	generar_txt();
	
	subrayado(n);
	
	gtk_text_buffer_get_bounds(buffer[n],&start,&end);
	char *text = gtk_text_buffer_get_text(buffer[n],&start,&end,-1);
	if(may_pun(text))
        gtk_text_buffer_set_text(buffer[n],text,-1);

	

		
	if(cont_lineas(n)>=23 && n<=2){
		extra(n);
	}
	else if(cont_lineas(3)>=23 && cont==4){
		GtkTextIter aux_max,end_max;
		gboolean status;
		int n1=0;
		char *texto_extra;
		gtk_text_buffer_get_start_iter(buffer[3],&aux_max);
		
		do{
			if(gtk_text_view_starts_display_line(texto[3],&aux_max)==TRUE){
				++n1;
				if(n1==23)
					break;	
				
			}
			
			status = gtk_text_view_forward_display_line(texto[3],&aux_max);
		} while(status);	
		gtk_text_buffer_get_end_iter(buffer[3],&end_max);
		texto_extra = gtk_text_buffer_get_text(buffer[3],&aux_max,&end_max,-1);
		gtk_text_buffer_delete(buffer[3],&aux_max,&end_max);
			
			
		char *aux = g_convert("Mucha información, ya se llenaron las 4 páginas.",-1, "UTF-8", "Windows-1252", 0,0, NULL);
		GtkMessageDialog *dialog = gtk_message_dialog_new_with_markup (window,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,aux);
		gtk_window_set_modal(dialog, TRUE);
		crear_css(dialog,e_dialogo_mensaje);
		g_signal_connect(dialog,"response",ayuda_salida,NULL);

	// Establecer la ventana como la ventana principal del diálogo
		gtk_window_set_transient_for(dialog,window);
		gtk_widget_show(dialog);
	
	}
	
	scroll(n);
	
	return G_SOURCE_REMOVE;
}



//quitar una pag cuando sea necesario
void borrar_pag(gpointer user_data){
	
	int *p = (int *)user_data;
    int n = *p;
	
	GtkTextIter start,end,end2;
	gtk_text_buffer_get_bounds(buffer[n],&start,&end);
	char *pag;
	pag = gtk_text_buffer_get_text(buffer[n],&start,&end,-1);
	if(strlen(pag)<1 && n>0){
		crear_css(texto[n],e_hoja_bloqueada);	
		gtk_widget_set_size_request(texto[n],600,0);	
		//posicionar en la ultima posicion de pag anterior
		gtk_text_buffer_get_end_iter(buffer[n-1],&end2);
		gtk_text_buffer_place_cursor(buffer[n-1],&end2);
		gtk_widget_grab_focus(texto[n-1]);
		--cont;
	//para reasignar los buffers
	if(n>0 && n<cont-1){
		for(int k=n;k<4;++k){
			texto[n]=texto[n+1];
		}
	}
	}
	return G_SOURCE_REMOVE;
}



//funcion que llama al seleccionador de archivos y/o rutas
void encriptacion(GtkButton *boton_menu,gpointer user_data){
	
	encriptado = !encriptado;
	for(int k=cont-1;k>=0;--k){
		GtkTextIter in_cript,fin_cript;
		gtk_text_buffer_get_bounds(buffer[k],&in_cript,&fin_cript);
		char *mensaje =  gtk_text_buffer_get_text(buffer[k],&in_cript,&fin_cript,-1);
		
		if(!strlen(mensaje))
			break;
		
		encriptado ? encriptar(mensaje,k) : desencriptar(mensaje,k);
		if(encriptado)
			encriptado_txt();
		else
			remove("encriptado.txt");
	}
	
	gtk_popover_popdown(popover[0]);
	
	for(int k=0;k<4;++k){
		gtk_text_view_set_editable(texto[k],!encriptado);
	}
	
}

//funcion que borra texto de programa y de txt
void borrar_texto(GtkButton *boton_menu,gpointer user_data){
	encriptado = FALSE;
	cont=1;
	for(int k=0;k<4;++k){
		
		GtkTextIter inicio, fin;
		gtk_text_buffer_get_bounds(buffer[k],&inicio,&fin);
		gtk_text_buffer_delete(buffer[k],&inicio,&fin);
		if(k>0){
			crear_css(texto[k],e_hoja_bloqueada);
			gtk_widget_set_size_request(texto[k],600,0);
		}
		
		gtk_text_view_set_editable (texto[k],!encriptado);
	}
	gtk_popover_popdown(popover[0]); //quitar popover de vista
}

//funcion que tiene la guia del programa
void guia(GtkButton *boton_menu,gpointer user_data){

	gtk_popover_popdown(popover[1]); //quitar popover apenas 
	char *aux = g_convert("DETALLES DE LA APLICACIÓN:\n\n- Este es una aplicación para escribir texto en hojas.\n\n- Se puede escribir hasta un máximo de 4 hojas.\n\n- Cada hoja puede tener hasta 22 líneas.\n\n- El texto se justifica por defecto.\n\n- Las palabras mal escritas saldrán subrayadas.\n\n- Al hacer doble click izquierdo en una palabra se puede corregir.\n\n- La información se guarda automáticamente en un txt llamado contenido.txt.\n\n- Si le da a 'Opción' y a 'Enciptar' toda la información del programa será encriptada.",-1, "UTF-8", "Windows-1252", 0,0, NULL);
	GtkMessageDialog *dialog = gtk_message_dialog_new_with_markup (window,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,aux);
		
	crear_css(dialog,e_dialogo_ayuda);
		
		
		// Establecer el diálogo como modal
	gtk_window_set_modal(dialog, TRUE);
	g_signal_connect(dialog,"response",ayuda_salida,NULL);
		// Establecer la ventana como la ventana principal del diálogo
	gtk_window_set_transient_for(dialog, window);
	gtk_widget_show(dialog);
}

//funcion que se encarga de mostrar los popovers de la barra de menu de arriba
void mostrar_menus(gpointer user_data){
	GtkBox *box_menu[2]; 
	GtkButton *btn_menu[2];
	GtkTextIter in,fin;
	int *p = (int *)user_data;
	int n= *p,total_info=0;
	
	popover[n]=gtk_popover_new();
	gtk_widget_set_parent(popover[n],boton[n]);
	box_menu[n]=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_popover_set_child(popover[n],box_menu[n]);

	for(int k=0;k<4;++k){
	    gtk_text_buffer_get_bounds(buffer[k],&in,&fin);
        gchar *info = gtk_text_buffer_get_text(buffer[k],&in,&fin,-1);
        total_info+=strlen(info);
	}
	
	if(n==0){
		btn_menu[0]=gtk_button_new_with_label(!encriptado ? "Encriptar" : "Desencriptar");
		g_signal_connect(btn_menu[0],"clicked",encriptacion,NULL);
		gtk_widget_set_sensitive(btn_menu[0], total_info>0 );
		gtk_box_append(box_menu[n],btn_menu[0]);
		
		btn_menu[1]=gtk_button_new_with_label("Borrar texto");
		g_signal_connect(btn_menu[1],"clicked",borrar_texto,NULL);
		gtk_widget_set_sensitive(btn_menu[1], (!encriptado && total_info>0) );
		gtk_box_append(box_menu[n],btn_menu[1]);
		
	}
	if(n==1){
		char *aux = g_convert("Guía de uso",-1, "UTF-8", "Windows-1252", 0,0, NULL);
		btn_menu[0]=gtk_button_new_with_label(aux);
		g_signal_connect(btn_menu[0],"clicked",guia,NULL);
		gtk_box_append(box_menu[n],btn_menu[0]);
	}
	for(int k=0;k<(n==0 ? 2 : 1);++k)
		crear_css(btn_menu[k],e_boton_boton_barra);

		
	gtk_widget_show(popover[n]);
	gtk_popover_popup(popover[n]);
		
}

//funcion que cambia la palabra mal escrita por la sugerencia
void corregir(GtkButton *button,gpointer data){
	int *p = (int *)data;
	int n = *p;
	gtk_text_buffer_delete(buffer[n],&inicio,&fin);
	
	gchar *correccion = gtk_button_get_label(button);

	gtk_text_buffer_insert(buffer[n],&inicio,correccion,-1);
	gtk_popover_popdown (m_corr);
	
	
}

void agregar_palabra(GtkButton *button, gpointer user_data){
    int *p = (int *)user_data;
    int n=*p;
	char *palabra = gtk_text_buffer_get_text(buffer[n],&inicio,&fin,-1);
    FILE *file = fopen("es-ve.dic", "r");
    FILE *temp = fopen("temp.dic", "w");
	
    char buffer[256];
    int numero;
    fscanf(file, "%d", &numero);  // Read the first line
    fprintf(temp, "%d", numero+1);  // Write the new line to the temp file

    while (fgets(buffer, sizeof(buffer), file)) {
        fprintf(temp, "%s", buffer);  // Copy the rest of the file
    }
    fprintf(temp,"%s\n",palabra);

    fclose(file);
    fclose(temp);
    remove("es-ve.dic");
    rename("temp.dic","es-ve.dic");
    
    Hunspell_destroy(hunspell);
    hunspell = Hunspell_create("es-ve.aff","es-ve.dic");
    
	subrayado(n);

	gtk_popover_popdown (m_corr);
}


//funcion para detectar doble click derecho en la palabra que busco
void click(GtkGestureClick *gesture,int n_press,double x, double y,gpointer data){
	//bloquea el corrector si esta encriptado
	if(encriptado || n_press!=2)
		return;

	
	
	int *p = (int *)data;
	int n= *p,n_sug;
	

	
    GtkBox *caja_menu;
    GtkButton *btn_sug[8],*nuevo;
    GtkLabel *label;
    
    GtkTextIter iter;
    gtk_text_view_get_iter_at_location(texto[n], &iter, x, y);

    inicio = fin = iter;
	//determina que palabra se clickeo
    if (!gtk_text_iter_starts_word(&inicio))
        gtk_text_iter_backward_word_start(&inicio);
        
    if (!gtk_text_iter_ends_word(&fin))
        gtk_text_iter_forward_word_end(&fin);

    char *word = gtk_text_buffer_get_text(buffer[n],&inicio, &fin,-1);
    
    char *cadena = g_locale_from_utf8(word,-1,NULL,NULL,NULL);
				

    //bucle para saber si es una palabra y pasar a minuscula las mayusculas
	if(strlen(word)==0){
		return;
	}
    for(int k=0;k<strlen(cadena);++k){
    	if(!isalpha(cadena[k]) && (cadena[k]!='á' && cadena[k]!='é' && cadena[k]!='í' && cadena[k]!='ó' && cadena[k]!='ú' && cadena[k]!='ñ' && cadena[k]!='ü'&& cadena[k]!='Á' && cadena[k]!='É' && cadena[k]!='Í' && cadena[k]!='Ó' && cadena[k]!='Ú' && cadena[k]!='Ñ' && cadena[k]!='Ü'))
    		return;
    		
	}
	if(gtk_text_iter_get_char(&fin)==' ' || gtk_text_iter_get_char(&fin)=='\n'){
		if(!Hunspell_spell(hunspell,word)){

            char **suggestions;

            int num_suggestions = Hunspell_suggest(hunspell, &suggestions, word); 
			gtk_widget_unparent(m_corr);
			m_corr = gtk_popover_new();
			
			caja_menu = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
			gtk_widget_set_parent(m_corr,texto[n]);
			gtk_popover_set_child(m_corr,caja_menu);
			gtk_popover_set_has_arrow(m_corr,FALSE);
			
			if(num_suggestions>0){
				label = gtk_label_new("Sugerencias:\n ");
				gtk_box_append(caja_menu,label);
				if(num_suggestions>8)
			 	    num_suggestions=8;
				for(int k=0;k<num_suggestions;++k){
					btn_sug[k]= gtk_button_new_with_label(suggestions[k]);
					crear_css(btn_sug[k],boton_sugerencia);
					gtk_box_append(caja_menu,btn_sug[k]);
					g_signal_connect(btn_sug[k],"clicked",corregir,&pos_pag[n]);
					
				}			
			}
			else{
				label = gtk_label_new("No hay sugerencias disponibles");
				gtk_box_append(caja_menu,label);
			}
			if(strlen(word)<50){
                nuevo = gtk_button_new_with_label("Agregar palabra");
			    crear_css(nuevo,boton_agregar);
                gtk_box_append(caja_menu,nuevo);
			    g_signal_connect(nuevo,"clicked",agregar_palabra,&pos_pag[n]);
			}
			//Gdkrectangle se encarga de posicionarse en el widget
			GdkRectangle rect1;
			rect1.x =(int) x;			
			rect1.y =(int) y;			
			rect1.width = 1;			
			rect1.height = 1;	
					
			crear_css(m_corr,e_popover);
			//indica donde se apunta
			gtk_popover_set_pointing_to(m_corr,&rect1);	
			//indica donde se ubica
			gtk_popover_set_position(m_corr,GTK_POS_BOTTOM);
			
			gtk_widget_show(m_corr);
			gtk_popover_popup(m_corr);
			
		}
	}
    
        g_free(word);
}



void mensaje_inicial(){
    char *mensaje =  g_convert("Bienvenido usuario, si quiere saber detalles de la aplicación lea las instrucciones del menú 'Ayuda'", -1, "UTF-8", "Windows-1252", 0,0, NULL);
	GtkMessageDialog *dialog = gtk_message_dialog_new_with_markup (window,GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,mensaje);
		
	crear_css(dialog,e_dialogo_mensaje);
	
		// Establecer el diálogo como modal
	gtk_window_set_modal(dialog, TRUE);
	g_signal_connect(dialog,"response",ayuda_salida,NULL);
		// Establecer la ventana como la ventana principal del diálogo
	gtk_window_set_transient_for(dialog,window);
	gtk_widget_show(dialog);
}


//funcion donde se crean la mayoria de widgets y configuraciones
void activar(GtkApplication *app, gpointer user_data)
{
	
  	GtkBox *box_pag,*box_prin,*box_btns;
  	GtkGesture *gesto[4]; //detectar posicion click

//	window y windowscrolled
	window = gtk_application_window_new (app);
	gtk_window_set_title (window, "Proyecto de Laboratorio");
	//dimesion default
	gtk_window_set_default_size (window, 800, 600);
	gtk_window_maximize(window);
	//dimension minima
	gtk_widget_set_size_request (window, 800, 600);
	GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons (header,TRUE);
    gtk_window_set_titlebar(window, header);
    crear_css(header,barra);
	ventana_scrolled = gtk_scrolled_window_new();
	
	
	crear_css(window,e_ventana);
	
	
//	boxes del programa                                
  	box_pag = gtk_box_new (GTK_ORIENTATION_VERTICAL,10);
  	box_prin = gtk_box_new (GTK_ORIENTATION_VERTICAL,0);
	box_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	
	//estilo de box de botones
	crear_css(box_btns,e_barra);
	
	//creacion y configuracion de botones de arriba 
	boton[0]=gtk_button_new_with_label("Opciones");
	boton[1]=gtk_button_new_with_label("Ayuda");
	
	for(int k=0;k<2;++k){
		
		gtk_box_append(box_btns,boton[k]);
		
		crear_css(boton[k],e_boton_barra);
		
		g_signal_connect_swapped(boton[k],"clicked",mostrar_menus,&pos_btns[k]);
		
		
	}
	
	gtk_box_append(box_prin,box_btns);
	//creacion de las 4 paginas
	
	for(int k=0;k<4;++k){
		texto[k] = gtk_text_view_new();
		gesto[k]= gtk_gesture_click_new();
																	                                           
		//ajustes principales de text_view
		gtk_widget_set_size_request(texto[k],600,k==0 ? 600: 0);
		
		gtk_text_view_set_justification(texto[k],GTK_JUSTIFY_FILL);
		gtk_text_view_set_accepts_tab(texto[k],TRUE);
		gtk_text_view_set_wrap_mode(texto[k],GTK_WRAP_WORD_CHAR);
		gtk_widget_set_halign(texto[k],GTK_ALIGN_CENTER);

		//PROCESO PARA HACER FUNCIONAR EL TAB CON EL WRAP MODE
		GtkEventController *controller = gtk_event_controller_key_new();
		g_signal_connect(controller, "key-pressed",teclas, &pos_pag[k]);
		gtk_widget_add_controller(texto[k], controller);

		
		gtk_widget_add_controller(texto[k],gesto[k]);
		
		buffer[k] = gtk_text_view_get_buffer(texto[k]);
		
		//uso swapped solo porque me interesa saber el numero de pagina, no necesito widgets
		g_signal_connect_swapped(buffer[k],"changed",cambio_buffer,&pos_pag[k]); //señal de text_view
		g_signal_connect_swapped(texto[k],"backspace",borrar_pag,&pos_pag[k]); //señal de text_view
	
		g_signal_connect(gesto[k],"released",click,&pos_pag[k]);
	
		crear_css(texto[k],k==0 ? e_hoja : e_hoja_bloqueada);
		
		
		gtk_box_append(box_pag,texto[k]);
		
		gtk_widget_set_visible(texto[k],TRUE);
			
	}
	
	//Asignaciones de hijos a widgets principales
	gtk_scrolled_window_set_child(ventana_scrolled,box_pag);
	gtk_widget_set_vexpand(ventana_scrolled,TRUE);
	gtk_box_append(box_prin,ventana_scrolled);
	gtk_window_set_child(window,box_prin);
  	gtk_widget_set_visible(window,TRUE);
  	


	gtk_widget_grab_focus(texto[0]);
	
  	
//	mensaje_inicial();
  	
}


//funcion que lanza la aplicacion
int main (int argc,char **argv)
{
	setlocale(LC_ALL,"");

	hunspell = Hunspell_create("es-ve.aff","es-ve.dic");

  	GtkApplication *app;
  	app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  	g_signal_connect (app, "activate",activar, NULL);
  	g_application_run (G_APPLICATION (app), argc, argv);
  	
  	return 0;
}


//este archivo tiene la funcion que se encarga de todo lo relativo a subrayado

void subrayado(int n){
	gboolean estado; 
	GtkTextIter start,end,start2,inicio_sub,fin_sub;
	gtk_text_buffer_get_bounds(buffer[n],&start,&end);
	
	
	//subrayado de palabra mal escritas
	inicio_sub=fin_sub=start;
	//creacion de texttag
	GtkTextTag *tag_underline = gtk_text_tag_new(NULL); 
	//propiedad de subrayadado a tag
	g_object_set(G_OBJECT(tag_underline), "underline", PANGO_UNDERLINE_ERROR, NULL);   
	//creacion de texttagtable
	GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer[n]);
	//se remueve y agrega el tag
	gtk_text_buffer_remove_all_tags(buffer[n], &start, &end);
	gtk_text_tag_table_add(tag_table, tag_underline);
	
	do{
		if(gtk_text_iter_starts_word(&inicio_sub)){
			gtk_text_iter_forward_word_end(&fin_sub);
			if(gtk_text_iter_get_char(&fin_sub)==' ' ||gtk_text_iter_get_char(&fin_sub)=='\n' ){
				
				char *word = gtk_text_buffer_get_text(buffer[n],&inicio_sub, &fin_sub,-1);
			 	
				char *cadena;
				
				cadena = g_locale_from_utf8(word,-1,NULL,NULL,NULL);
				
				    for(int k=0;k<strlen(cadena);++k){
				    	if(!isalpha(cadena[k]) && (cadena[k]!='á' && cadena[k]!='é' && cadena[k]!='í' && cadena[k]!='ó' && cadena[k]!='ú' && cadena[k]!='ñ' && cadena[k]!='ü'))
				    		return;
				    	else if(isalpha(cadena[k]) && k!=0)
				    		cadena[k]=tolower(cadena[k]);
					}
				
				
				if(!Hunspell_spell(hunspell,word)){
					gtk_text_buffer_apply_tag(buffer[n], tag_underline, &inicio_sub, &fin_sub);
					inicio_sub=fin_sub;
				}
			}
			
			
		}
		estado = gtk_text_iter_forward_char(&inicio_sub);
	} while(estado);
}



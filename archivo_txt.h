
void lineas(int index){
	// Get the start iter
	GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer[index], &iter);
	int k=0;
	
	for(int k=0;k<4;++k){
		for(int l=0;l<1000;++l)
			lineas_txt[k][l][0]='\0';
		num_lineas[k]=0;
		
	}
	
    // Loop over each displayed line
    while (!gtk_text_iter_is_end(&iter) && k<1000) {
        GtkTextIter start, end;

        // The start of the line is where the iter currently points
        start = iter;

        // Move the iter to the start of the next displayed line
        if (!gtk_text_view_forward_display_line(texto[index],&iter)) {
            // If we couldn't move to the next line, move to the end of the buffer
            gtk_text_buffer_get_end_iter(buffer[index], &iter);
        }

        // The end of the line is where the iter now points
        end = iter;

        // Get the text of the line
        strcpy(lineas_txt[index][k],gtk_text_buffer_get_text(buffer[index], &start, &end, FALSE));
        // Store the line text in the array
        // Free the line text

		if(lineas_txt[index][k][strlen(lineas_txt[index][k])-1]=='\n')
			lineas_txt[index][k][strlen(lineas_txt[index][k])-1]='\0';
	
        ++k;
        ++num_lineas[index];
	}
}

void dibujar_borde(FILE *file){
	fprintf(file," ");
	for(int i=0;i<64;i++){
		fprintf(file,"*");
	}
	
	fprintf(file,"\n");
}
void dibujar_lineas(FILE *file,int index){
	int extra, contador=0;
	
	for(int i=0;i<num_lineas[index];++i){
		
			contador=0;
			fprintf(file," * %s",lineas_txt[index][i]);
			for(int k=0;k<strlen(lineas_txt[index][i]);++k){
				if(lineas_txt[index][i][k]=='\t')
					++contador;
			}
			extra = 61-strlen(lineas_txt[index][i])- (4*contador);
			for(int j=0;j<extra;j++)
				fprintf(file," ");
			
			fprintf(file,"*\n");
		}

				

	
}

void generar_txt(){
	contenido = fopen( "contenido.txt","w");
	for(int k=0;k<cont;++k){
		lineas(k);
		dibujar_borde(contenido);
		dibujar_lineas(contenido,k);
		dibujar_borde(contenido);
		
	}
	fclose(contenido);

	
}
void encriptado_txt(){
	guardado = fopen("encriptado.txt","w");
	for(int k=0;k<cont;++k){
		lineas(k);
		dibujar_borde(guardado);
		dibujar_lineas(guardado,k);
		dibujar_borde(guardado);
		}
	
	fclose(guardado);
}





void encriptar(char *cad,int index){
	char *cadena = g_locale_from_utf8(cad,-1,NULL,NULL,NULL);
	int A[3][3] = {{1,-2,2},{-1,1,3},{1,-1,-4}};
	int longitud = strlen(cadena);
	int cont =0,sum=0;
	char cad_en[20000];
	cad_en[0]='\0';
	int long_matriz = longitud%3==0 ? longitud/3 : (longitud/3)+1;
	int nums[long_matriz][3], nums_en[long_matriz][3],nums_desen[long_matriz][3];
	for(int j=0; j<long_matriz;++j){
		for(int k=0; k<3;++k){
			nums[j][k] = cadena[cont] ? cadena[cont] : 32;
			++cont;
		}
	}

	for(int j = 0;j<long_matriz;++j){
		for(int k=0; k<3;++k){
			for(int l = 0;l<3;++l)
				sum+=nums[j][l]*A[l][k];
			
			nums_en[j][k]=sum;
			sum=0;
		}
	}
	
	for(int j=0;j<long_matriz;++j){
		for(int k=0;k<3;++k){
			char temp[12];
			sprintf(temp,"%d ",nums_en[j][k]);
			strcat(cad_en,temp);
			
		}
	}

	gchar *cadena_utf8 = g_convert(cad_en, -1, "UTF-8", "Windows-1252", 0,0, NULL);
	gtk_text_buffer_set_text(buffer[index],cadena_utf8,-1);
	cad_en[0]='\0';
	
}

void desencriptar(char str[],int index){
    int numbers[20000][3],nums_des[20000][3]; // Make sure the array is large enough to hold all the numbers
    int i = 0,sum=0,contador=0;
    int A_inv[3][3] = {{-1,-10,-8},{-1,-6,-5},{0,-1,-1}};
    char cad_des[2000];
    char *token = strtok(str, " ");
    while(token != NULL) {
    	for(int j=0;j<3;++j){
        	numbers[i][j] = atoi(token);
        	token = strtok(NULL, " ");
		}
		++i;
		
    }

    
	for(int j = 0;j<i;++j){
		for(int k=0; k<3;++k){
			for(int l = 0;l<3;++l){
				
				sum+=numbers[j][l]*A_inv[l][k];
			}
			nums_des[j][k]=sum;
			sum=0;

			cad_des[contador]=nums_des[j][k];
			++contador;
		}
	}
	if(cad_des[contador-2]==' ')
 	    cad_des[contador-2]='\0';
	if(cad_des[contador-1]==' ')
 	    cad_des[contador-1]='\0';
	gchar *cadena_utf8 = g_convert(cad_des, -1, "UTF-8", "Windows-1252", 0,0, NULL);
	gtk_text_buffer_set_text(buffer[index],cadena_utf8,-1);
}


//funcion para determinar cual es el minimo de los 3
int min(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}
//funcion que calcula cercania de dos palabras
int levenshtein(char *s1, char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    if(abs(len1-len2)>4)
    	return 10; //valor alto para que sea ignorado en sugerencias
    int matriz[len1 + 1][len2 + 1];
    int j;
    for (j = 0; j <= len1; j++) {
        matriz[j][0] = j;
    }
    for (j = 0; j <= len2; j++) {
        matriz[0][j] = j;
    }
    for (j = 1; j <= len1; j++) {
        
        char c1;
        c1 = s1[j-1];
        for (int k = 1; k <= len2; k++) {
            char c2;
            c2 = s2[k-1];
            if (c1 == c2) {
                matriz[j][k] = matriz[j-1][k-1];
            }
            else {
                matriz[j][k] = min(matriz[j-1][k], matriz[j][k-1], matriz[j-1][k-1]) + 1;
            }
        }
    }
    return matriz[len1][len2];
}
//funcion para determinar si una palabra existe para no mostrar sugerencias
int existe(char *palabra, FILE *archivo){
	char linea[100];
	int status;
	rewind(archivo);
	do{
		status = fgets(linea,sizeof(linea),archivo);
		if(status){
			linea[strlen(linea)-1]='\0';
			if(levenshtein(palabra,linea)==0){
				return 1;
			}	
		}
		
	} while(status);
	
	return 0;
}
//funcion para guardar sugerencias de palabra mal escrita en un arreglo de cadenas segmentado en grados de cercania
int sugerencia(char *palabra,FILE *archivo,char *sugerencias[4]){
	int status,sim,conts[4]={0,0,0,0},k=0,l,m,n=0;
	rewind(archivo);

	do{
		status = fgets(l_txt[n],sizeof(l_txt[n]),archivo);
		if(status!=0){
			l_txt[n][strlen(l_txt[n])-1]='\0';
			sim = levenshtein(palabra,l_txt[n]);
			if(sim<=4 && conts[sim-1]<4){
				s[sim-1][conts[sim-1]]=l_txt[n];
				++conts[sim-1];
				++n;
				if(n==16 || conts[0]==4){
					break;
				}
			}
		}
	} while(status);
	for(l=0;l<4;++l){
		for(m=0;m<4;++m){
			if(strlen(s[l][m])>0){
				sugerencias[k]=s[l][m];
				++k;
				if(k==4){
					l=10;
					break;
				}
			}
		}
	}
	return k;
}

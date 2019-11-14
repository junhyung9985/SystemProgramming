//
//  main.c
//  numbercross
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[]) {
    
    FILE * fp = fopen("formula", "w") ;
    FILE * fpc = NULL;
    int **a = NULL;
    
    if(argc>1)
           //printf("go");
           fpc = fopen(argv[1],"r");
     else{
           printf("not a form\n");
           exit(-1);}

    char buffer[100000];

     
    int row=0,col=0;
    
    while(1){
      fgets(buffer, sizeof(buffer), fpc);
      int already = 0;
      if(feof(fpc))break;
        for (int i =0; (row == 0) &&i < sizeof(buffer); i++){
            if(buffer[i] == ' '){already = 0; continue;}
            if(buffer[i] == '\n') break;
            if(already == 1){continue;}
            already = 1;
            col++;
            //printf("%c, col:%d \n", buffer[i], col);
        }
        row++;
        
     }
    col ++;
    //printf("row: %d col: %d", row, col);
    fclose(fpc);
    
    a=(int**)malloc((row+1) * sizeof (int*));
    for (int j = 0; j < row+1; j++)
    a[j]=(int*)malloc((col+1) * sizeof(int));
    //할당
    for (int i =0; i<row+1; i++){
           for (int j=0; j<col+1; j++){
               a[i][j] = 0;
               //printf("%d", a[i][j]);
           }
           //printf("\n");
       } // Debugging - 2
    
        FILE * fpd = fopen(argv[1],"r");
    /*   char buffer2 [90000];
       for (int i =0; i<90000;i ++){
           buffer2[i] = 0;
       }*/
       int k=1,l=1;
       int already = 0;
       
       char c = 0;
       while(1){
           c = fgetc(fpd);
           if(feof(fpd)) break;
           if(c==' ') {l++; already = 0; continue;}
           if(c=='\n'){k++; l=1; continue;}
           if(c!='\0'){
                   if(c=='?'){
                       //printf("-1 ");
                       a[k][l]=0;
                       //l++;
                   }else{
                       //printf("%d ",atoi(&c));
                       a[k][l]*=10;
                       a[k][l]+=atoi(&c);
                       already = 1;
                   }
               
           }
       }
    
    fclose(fpd);
  /*
    printf("\n \n");
     for(int i=0;i<row+1;i++){
     for(int j=0;j<col+1;j++)
         printf("%d ",a[i][j]);
     printf("\n");
     }
     printf("\n \n");
    */
  /*  for (int i =2; i<row+1 ; i++){
        for (int j=1; j<col; j++){
            fprintf(fp, "(declare-const b%d_%d Int)\n", i, j) ;
            fprintf(fp, "(assert(or (= b%d_%d 1) (= b%d_%d 0)))\n", i, j, i, j);
        }
    }
    
    for (int j =1; j<col; j++){
        fprintf(fp, "(assert(= (+ ");
        for (int i=2; i<row+1;i++){
            fprintf(fp, "(* %d b%d_%d) ", a[i][j],i,j);
        }
        fprintf(fp,") %d))\n", a[1][j]);
    }
    for (int i =2; i<row+1; i++){
        fprintf(fp, "(assert(= (+ ");
        for (int j=1; j<col;j++){
            fprintf(fp, "(* %d (- 1 b%d_%d)) ", a[i][j],i,j);
        }
        fprintf(fp, ") %d))\n", a[i] [col]);
    }
    */
    for (int i = 1 ; i < row+1 ; i++)
        for (int j = 1 ; j < col+1 ; j++){
            if(i == 1 && j == col) continue;
            fprintf(fp, "(declare-const a%d_%d Int)\n", i, j) ;
        }
    for (int i =1; i<row+1; i++)
        for (int j=1; j<col+1; j++){
            if(i ==1 && j==col) continue;
            fprintf(fp, "(assert (= a%d_%d %d))\n", i, j, a[i][j]);
        }
    for (int i =2; i<row+1 ; i++){
        for (int j=1; j<col; j++){
            fprintf(fp, "(declare-const b%d_%d Int)\n", i, j) ;
            fprintf(fp, "(assert(or (= b%d_%d 1) (= b%d_%d 0)))\n", i, j, i, j);
            fprintf(fp, "(assert(and (<= a%d_%d 9) (<= 1 a%d_%d)))\n", i, j,i,j);
        }
    }
    
    for (int j =1; j<col; j++){
        fprintf(fp, "(assert(= (+ ");
        for (int i=2; i<row+1;i++){
            fprintf(fp, "(* a%d_%d b%d_%d) ", i, j,i,j);
        }
        fprintf(fp,") a1_%d))\n", j);
    }
    for (int i =2; i<row+1; i++){
        fprintf(fp, "(assert(= (+ ");
        for (int j=1; j<col;j++){
            fprintf(fp, "(* a%d_%d (- 1 b%d_%d)) ", i, j,i,j);
        }
        fprintf(fp, ") a%d_%d))\n", i, col);
    }
    
    fprintf(fp,"(check-sat)\n(get-model)\n");
      fclose(fp) ;
    
    FILE * fin = popen("z3 formula", "r") ; //FIXME
       int board[row+1][col+1] ;
       int i, j;

       char b[128] ;
       char s[128] ;
       char position, close ;
       int value = 0;
       
       fscanf(fin, "%s %s", b, s) ;
       if(strcmp(b,"unsat")==0){
           printf("No solution!\n");
           exit(-1);
       }
       
    while(!feof(fin)) {
         fscanf(fin,"%s %c%d%c%d %s %s", b, &position, &i, &close, &j, b, b);
         fscanf(fin, " %d%c", &value, &close );
         //printf("%c%d %d%c\n", position, where, value, close);
        if (position == 'b')
         board[i][j] = value;
         //printf("%d %d %d %d\n", where, i, j, value);
     }
     
     for (i = 2 ; i <= row ; i++) {
             for (j = 1 ; j <= col-1 ; j++) {
                     printf("%d ", board[i][j]) ;
             }
             printf("\n") ;
     }
     pclose(fin) ;
    return 0;
}

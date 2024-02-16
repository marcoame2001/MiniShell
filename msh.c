//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8


// ficheros por si hay redirección
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Saliendo del MSH **** \n");
	//signal(SIGINT, siginthandler);
        exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
    //reset first
    for(int j = 0; j < 8; j++)
        argv_execvp[j] = NULL;

    int i = 0;
    for ( i = 0; argvv[num_command][i] != NULL; i++)
        argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
    /**** Do not delete this code.****/
    int end = 0; 
    int executed_cmd_lines = -1;
    char *cmd_line = NULL;
    char *cmd_lines[10];

    if (!isatty(STDIN_FILENO)) {
        cmd_line = (char*)malloc(100);
        while (scanf(" %[^\n]", cmd_line) != EOF){
            if(strlen(cmd_line) <= 0) return 0;
            cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
            strcpy(cmd_lines[end], cmd_line);
            end++;
            fflush (stdin);
            fflush(stdout);
        }
    }

    /*********************************/

    char ***argvv = NULL;
    int num_commands;


	while (1) 
	{
		int status = 0;
	        int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

		// Get command
                //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
                executed_cmd_lines++;
                if( end != 0 && executed_cmd_lines < end) {
                    command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
                }else if( end != 0 && executed_cmd_lines == end)
                    return 0;
                else
                    command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
                //************************************************************************************************


              /************************ STUDENTS CODE ********************************/
            if (command_counter > 0) 
            {
                if (command_counter > MAX_COMMANDS)
                    {
                        printf("Error: Numero máximo de comandos es %d \n", MAX_COMMANDS);
                    }
                
                else 
                {   

                    // CREAMOS LAS PIPES ENTRE PROCESOS HIJOS: 

                    int pipes[7][2]; // Vector de Pipes(que son vectores)

                    int fd1[2]; 
                    pipe(fd1);
                    pipes[0][0] = fd1[0];
                    pipes[0][1] = fd1[1];

                    int fd2[2];
                    pipe(fd2);
                    pipes[1][0] = fd2[0];
                    pipes[1][1] = fd2[1];
                    
                    int fd3[2];
                    pipe(fd3);
                    pipes[2][0] = fd3[0];
                    pipes[2][1] = fd3[1];

                    int fd4[2];
                    pipe(fd4);
                    pipes[3][0] = fd4[0];
                    pipes[3][1] = fd4[1];

                    int fd5[2];
                    pipe(fd5);
                    pipes[4][0] = fd5[0];
                    pipes[4][1] = fd5[1];

                    int fd6[2];
                    pipe(fd6);
                    pipes[5][0] = fd6[0];
                    pipes[5][1] = fd6[1];

                    int fd7[2];
                    pipe(fd7);
                    pipes[6][0] = fd7[0];
                    pipes[6][1] = fd7[1];   

                    // COMANDOS INTERNOS MYCALC Y MYCP

                    int mycalc_equal = 0;
                    int mycp_equal = 0;

                    if (command_counter == 1) 
                    { 
                        //MyCalc

                        char *mycalc = "mycalc"; 
                        mycalc_equal = 1;

                        for (int i = 0; (i < 7) && (mycalc_equal == 1); i++ )
                        {
                            if (argvv[0][0][i] != mycalc[i]) 
                            {
                                mycalc_equal = 0;
                            }
                        }
                        
                        if (mycalc_equal == 1)
                        {                           
                            char* Acc = getenv("Acc"); // Variable de entorno
                            int op1;
                            int op2;    
                            int sol;

                            int Acc_int = atoi(Acc); // Convierte la var. entorno de char a int

                            char *add = "add"; 
                            int add_equal = 1; 
                            
                            char *mod = "mod"; 
                            int mod_equal = 1;

                            if ( (argvv[0][1] != NULL) && (argvv[0][2] != NULL) && (argvv[0][3] != NULL) && (&(argvv[0][2][3]) != NULL)) //No faltan parametros, y arg 2 >= 3
                            {
                                // add

                                for (int i = 0; (i < 4) && (add_equal == 1); i++ ) // verifica si el argumento = add
                                {
                                    if (argvv[0][2][i] != add[i]) 
                                    {
                                        add_equal = 0;
                                    }
                                }

                                // mod

                                for (int i = 0; (i < 4) && (mod_equal == 1) && (&(argvv[0][2][i]) != NULL); i++ ) // verifica si el argumento = mod
                                {
                                    if (argvv[0][2][i] != mod[i]) 
                                    {
                                        mod_equal = 0;
                                    }
                                }
                                if (add_equal == 1) // add
                                {
                                    op1 = atoi(argvv[0][1]); 
                                    op2 = atoi(argvv[0][3]);
                                    
                                    Acc_int = op1 + op2 + Acc_int; // Valor de Acc actualizado
                                    sol = op1 + op2; // Valor de la operacion sin Acc

                                    sprintf(Acc,"%d",Acc_int); // Acc de int a char para poder modificar la var. entorno

                                    fprintf(stderr,"[OK] %i + %i = %i; Acc %i\n",op1,op2,sol,Acc_int); // Imprimir en caso de todo OK   
                                    setenv("Acc",Acc,1); // Actualizamos la variable de entorno Acc
                                }
                                
                                else if (mod_equal == 1) // mod
                                {
                                    op1 = atoi(argvv[0][1]); 
                                    op2 = atoi(argvv[0][3]);
                                    
                                    int resto = op1 % op2;   
                                    int cociente = op1 / op2;    
                                    char modu = '%';
                                
                                    fprintf(stderr,"[OK] %i %c %i = %i * %i + %i\n",op1,modu,op2,op2,cociente,resto); // Imprimir en caso de todo OK
                                }
                                
                                else // En caso de que el 2 argumento (add/mod) sea incorrecto (>=3 long)
                                {
                                    printf("[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>\n");
                                }
                            }
                            else // Faltan parametros o en caso de que el 2 argumento (add/mod) sea incorrecto (<3 long)
                            {
                                printf("[ERROR] La estructura del comando es <operando 1> <add/mod> <operando 2>\n");
                            }       
                        }

                        char *mycp = "mycp"; 
                        mycp_equal = 1;

                        for (int i = 0; (i < 5) && (mycp_equal == 1); i++ )
                        {
                            if (argvv[0][0][i] != mycp[i]) 
                            {
                                mycp_equal = 0;
                            }
                        }

                        if (mycp_equal == 1)
                        {
                            char buffer[1024];

                            int nread;

                            int error = 0;

                            int fb1 = open(argvv[0][1],O_RDONLY);
                            if (fb1 == -1) /*Si no se pudo abrir el archivo, imprime error*/
                            { 
                                write(1,"[ERROR] Error  al  abrir  el  fichero  origen\n",47); /*imprimimos el error mediante la llamada al sistema write*/
                                error = -1;
                            } 
                            if (error != -1)
                            {
                                int fb2 = open(argvv[0][2],O_WRONLY | O_CREAT, 0666);
                                
                                if (fb2 == -1) /*Si no se pudo abrir el archivo, imprime error*/
                                { 
                                    write(1,"[ERROR] Error  al abrir el fichero destino\n",44); /*imprimimos el error mediante la llamada al sistema write*/
                                    error = -1;
                                }     
                                if (error != -1)
                                {
                                    if ( (&argvv[0][1] == NULL) || (&argvv[0][2] == NULL))
                                    {
                                    write(1,"[ERROR] La estructura del comando es mycp <fichero origen> <fichero destino>\n",78); /*imprimimos el error*/
                                    error = -1;
                                    }
                                    if (error != -1)
                                    {
                                        while((nread = read(fb1, &buffer, 1024)) != 0)
                                        {
                                            write(fb2, &buffer, nread);
                                        }
                                        printf("[OK] Copiado con exito el fichero %s a %s\n",argvv[0][1],argvv[0][2]); // Imprimir en caso de todo OK 
                                    }
                                }
                            } 
                        }
                    }



                    // CREAMOS EL PROCESO INICIAL SI NO SE TRATA DE COMANDO INTERNO

                    if ( (mycalc_equal == 0) && (mycp_equal == 0) )
                    { 
                        pid_t pidC;

                        pidC = fork(); // Crea Hijo (Proceso Inicial)

                        if (pidC == -1) // Error
                        {
                            write(2,"Error: No se ha podido crear el hijo\n",38);
                            return -1;
                        }

                        else if (pidC == 0) // Hijo (Proceso Inicial)
                        {    
                            
                            if (*filev[0] != '0') // Si existe Redireccionamiento de Lectura ...
                            {
                                int fdinput = open(filev[0],O_RDONLY); // Redireccionamiento de Lectura 1.0
                                dup2(fdinput,STDIN_FILENO); // Redireccionamiento de Lectura 1.1
                            }                 
                        
                            if ( (*filev[1] != '0') && (command_counter == 1) ) // Si existe Redireccionamiento de Escritura y el Proceso Inicial = Proceso Final...
                            {
                                int fdout = open(filev[1], O_CREAT | O_WRONLY, 0666); // Redireccionamiento de Escritura 1.0
                                dup2(fdout,STDOUT_FILENO); // Redireccionamiento de Escritura 1.1                            
                            }
                        
                            if (*filev[2] != '0') // Si existe Redireccionamiento de Error ... 
                            {
                                int fdout = open(filev[2], O_CREAT | O_WRONLY, 0666); // Redireccionamiento de Error 1.0
                                dup2(fdout,STDERR_FILENO); // Redireccionamiento de Error 1.1                            
                            }
                        
                            if (command_counter > 1) // Si tiene procesos hermanos ... 
                            {
                                close(pipes[0][0]); // Cerramos Tuberia Lectura(0)
                                dup2(pipes[0][1],STDOUT_FILENO); // Tuberia Escritura(1) como Salida y Cerramos la Salida Estandar
                                close(pipes[0][1]); // Cerramos Tuberia Escritura(1)
                            }

                            execvp(argvv[0][0],argvv[0]); // Otros Procesos Cambio de Imagen 
                            write(2,"Si esto se ejecuta, se ha producido un error\n",46);
                            return -1;
                        }

                        else // Padre (Shell)
                        {

                            if (in_background == 0) // Si NO hay Background
                            {
                                while (wait(&status) != pidC); // Espera a que termine el Proceso Inicial         
                            }

                            if ( (command_counter == 1) && (in_background == 1) ) // Proceso Inicial = Proceso Final y Background ...
                            {
                                printf("PID = %i\n",(int)pidC); // Imprimimos el PID 
                            }
                            
                            // CREAMOS EL PROCESOS INTERMEDIOS

                            for (int i = 0; i < (command_counter - 2); i++) 
                            {
                                close(pipes[i][1]); // Cerramos Tuberia Escritura(1)
                                
                                pidC = fork(); // Crea Hijo (Proceso Intermedio)

                                if (pidC == -1) // Error
                                {
                                    write(2,"Error: No se ha podido crear el hijo\n",38);
                                    return -1;
                                }

                                else if (pidC == 0) // Hijo (Proceso Intermedio)
                                {
                                    if (*filev[2] != '0') // Si existe Redireccionamiento de Error ... 
                                    {
                                        int fdout = open(filev[2], O_CREAT | O_WRONLY, 0666); // Redireccionamiento de Error 1.0
                                        dup2(fdout,STDERR_FILENO); // Redireccionamiento de Error 1.1                            
                                    }

                                    close(pipes[i+1][0]); // Cerramos Tuberia Lectura(0)
                                    dup2(pipes[i][0],STDIN_FILENO); // Tuberia Lectura(0) como Entrada y Cerramos la Entrada Estandar
                                    close(pipes[i][0]); // Cerramos Tuberia Lectura(0)
                                    dup2(pipes[i+1][1],STDOUT_FILENO); // Tuberia Escritura(1) como Salida y Cerramos la Salida Estandar
                                    close(pipes[i+1][1]); // Cerramos Tuberia Escritura(1)

                                    execvp(argvv[i+1][0],argvv[i+1]); // Cambio de Imagen
                                    write(2,"Si esto se ejecuta, se ha producido un error",45);
                                    return -1;
                                }

                                else // Padre (Shell)
                                {
                                    close(pipes[i][0]); // Cerramos Tuberia Lectura(0)
                                    
                                    if (in_background == 0) // Si NO hay Background
                                    {
                                        while (wait(&status) != pidC); // Espera a que termine el Proceso Intermedio
                                    }
                                }
                            }                    
                            close(pipes[command_counter-2][1]); // Cerramos Tuberia Escritura(1)

                            // CREAMOS EL PROCESO FINAL
                            
                            if (command_counter > 1) // En caso de existir proceso final ...
                            {
                                pidC = fork(); // Crea Hijo (Proceso Final)

                                if (pidC == -1) // Error
                                {
                                    write(2,"Error: No se ha podido crear el hijo\n",38);
                                    return -1;
                                }

                                else if (pidC == 0) // Hijo (Proceso Final)
                                {
                                    if (*filev[1] != '0') // Si existe Redireccionamiento de Escritura ...
                                    {
                                        int fdout = open(filev[1], O_CREAT | O_WRONLY, 0666); // Redireccionamiento de Escritura 1.0
                                        dup2(fdout,STDOUT_FILENO); // Redireccionamiento de Escritura 1.1                            
                                    }
                                    
                                    if (*filev[2] != '0') // Si existe Redireccionamiento de Error ...  
                                    {
                                        int fdout = open(filev[2], O_CREAT | O_WRONLY, 0666); // Redireccionamiento de Error 1.0
                                        dup2(fdout,STDERR_FILENO); // Redireccionamiento de Error 1.1                            
                                    }
                                    
                                    dup2(pipes[command_counter-2][0],STDIN_FILENO); // Tuberia Lectura(0) como Entrada y Cerramos la Entrada Estandar
                                    close(pipes[command_counter-2][0]); // Cerramos Tuberia Escritura(1)
                                    execvp(argvv[command_counter-1][0],argvv[command_counter-1]); // Cambio de Imagen 
                                    write(2,"Si esto se ejecuta, se ha producido un error",45);
                                    return -1;

                                }

                                else // Padre (Shell)
                                {
                                    close(pipes[command_counter-2][0]);

                                    if (in_background == 0) // Si NO hay Background ...
                                    {
                                        while (wait(&status) != pidC); // Espera a que termine el Proceso Final
                                    }
                                    if (in_background == 1) // Si hay Background ...
                                    {
                                        printf("PID = %i\n",(int)pidC); // Imprimimos el PID
                                    }
                                }
                            }
                        }
                    }                                                                                             
                }
               
            }
        }
    return 0;
}


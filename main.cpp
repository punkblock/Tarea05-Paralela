#include <iostream>
#include <xlnt/xlnt.hpp>//leer excel
#include "xlsxwriter.h"//escribir excel
#include <string.h>
#include <cstdlib>
#include <string>
#include <sstream>//convertir int to string
#include <omp.h>
// include "mpi.h"

using namespace std;


/*********************************************************/

//CREAR ESTRUCTURAS PARA OCUPAR SOLO LA INFORMACION NECESARIA DEL ARREGLO

//Listo
typedef struct InfoSala {
    char nombreEdificio[30];
    char numeroSala[30];
    char sala[30];
    int lab;
    int codigoProfesor;
} InfoSala;

//Listo
typedef struct Horario {
  string bloque[7][6];
  char codigoRamo[20];
  int codigoProfesor;
  char sala[30];
  int restBlock[7];
} Horario;

//Listo
typedef struct Ramos{
  char codigoRamo[20];
  int codigoProfesor;
  int  horasRamo;//horas que tiene el ramo
  int estado=0;
} Ramos;

//Listo
typedef struct Profesor {
  int codigoProfesor;
  int diasBloques[7][6];
  int estado = 0;//0 aun quedan bloques
} Profesor;


/***********************FIN ESTRUCTURAS**********************************/

/***********************INICIO DE FUNCIONES**********************************/

Profesor profes[239];
void infoProfe(char *argumento){
    xlnt::workbook wb;
    wb.load(argumento);// Docentes.xlsx
    auto ws = wb.active_sheet();

	/* toda la hoja de cálculo */
	std::vector< std::vector<std::string> > hoja_calculo;
        for(const auto sheet : wb){ 
            for(auto row : sheet.rows()){
                vector<string> filas;
                for(auto cell : row){
                    filas.push_back(cell.to_string());
                }
                hoja_calculo.push_back(filas);
            }
        }

    int j=0;
    int k=0;
    int cont = 0;
    int cont2 = 0;
    int auxxx=1;
    int hoja=1;
    int dsp=0;//disponible
    string disp;
    string noDisp;
    string bloquee;
    for  (int fila = 1; fila < hoja_calculo.size(); fila++){
        if(fila == 240 || fila == 480 || fila == 720 || fila == 960 || fila == 1200){
            hoja++;
            cont=0;
            cont2=0;
            k++;//dia
        }else{
            if(cont<239){
                int codProfe = std::atoi (hoja_calculo.at(fila).at(0).c_str());
                profes[cont].codigoProfesor = codProfe;//guarda codProfe
                //cuenta profesores (desde 1 a 240)
                cont++;
            }
            if(fila<1201){
                j=0;//bloque
                for (int z=3; z<10; z++){//z: columnas
                    disp = "DISPONIBLE";
                    noDisp = "NO DISPONIBLE";
                    bloquee=hoja_calculo.at(fila).at(z).c_str();//guarda dia bloque
                    //cout << sss << endl;
                    if(bloquee == disp){
                        dsp=0;
                    }
                    if(bloquee == noDisp){
                        dsp=1;
                    }
                    if(cont2<239){
                        profes[cont2].diasBloques[j][k] = dsp; //z desde 4 a 10 excepto dia sabado
                    }
                    j++;
                }
            }
            if(fila>1200){
                j=0;//bloque
                for (int z=3; z<7; z++){//z: columnas
                    disp = "DISPONIBLE";
                    noDisp = "NO DISPONIBLE";
                    bloquee=hoja_calculo.at(fila).at(z).c_str();//guarda dia bloque
                    //cout << sss << endl;
                    if(bloquee == disp){
                        dsp=0;
                    }
                    if(bloquee == noDisp){
                        dsp=1;
                    }
                    if(cont2<239){
                        profes[cont2].diasBloques[j][k] = dsp; //z desde 4 a 10 excepto dia sabado
                        profes[cont2].diasBloques[4][5] = 2;
                        profes[cont2].diasBloques[5][5] = 2;
                        profes[cont2].diasBloques[6][5] = 2;
                    }
                    j++;
                }
            }
            cont2++;
            //profes[fila].estado = 1; // bloques y codigo cargado
        }
    }   
    std::cout << "FIN Docentes.xlsx" << endl;
}   

Ramos ramoss[346];
void infoRamos(char *argumento){
    xlnt::workbook wb;
    wb.load(argumento);// Cursos.xlsx
    int contador = 0;
    auto ws = wb.active_sheet();

	/* toda la hoja de cálculo */
	std::vector< std::vector<std::string> > hoja_calculo;
	std::vector< std::vector<std::string> > columna_c;
	for (auto row : ws.rows(false)){ 

		// Creando un vector nuevo solo para esta fila en la hoja de cálculo
		vector<string> fila_simple;

		for (auto cell : row){ 
		    //Añadiendo esta celda a la fila;
		    fila_simple.push_back(cell.to_string());
		}

		//Agregando esta fila completa al vector que almacena toda la hoja de cálculo;
		hoja_calculo.push_back(fila_simple);
        }

    int thread; //inicializacion variable para diferenciar hilo mientra se paraleliza
    #pragma omp parallel private(thread)
     
           {
          thread = omp_get_thread_num();
           #pragma omp for
           
    for (int fila = 1; fila < hoja_calculo.size(); fila++){
        string cRamo = hoja_calculo.at(fila).at(0);
        char cstr[cRamo.size() + 1];
	    strcpy(cstr, cRamo.c_str());
        strcpy(ramoss[fila].codigoRamo, cstr);// codigo ramo
	    
        int codProfe = std::atoi (hoja_calculo.at(fila).at(2).c_str());
        ramoss[fila].codigoProfesor = codProfe;
	    
        int horRamo = std::atoi (hoja_calculo.at(fila).at(5).c_str());
        ramoss[fila].horasRamo = horRamo;

    }  
           }
}

InfoSala salass[54];
void infoSalas(char *argumento){
      string lab="LAB";
      string esLab;
      xlnt::workbook wb;
      wb.load(argumento);// Salas.xlsx
      auto ws = wb.active_sheet();

    /* toda la hoja de cálculo */
    std::vector< std::vector<std::string> > hoja_sala;
    for(const auto sheet : wb){
        for(auto row : sheet.rows()){
            vector<string> filas;
            for(auto cell : row){
                filas.push_back(cell.to_string());
            }
            hoja_sala.push_back(filas);
        }
    }

    int thread; //inicializacion variable para diferenciar hilo mientra se paraleliza
    #pragma omp parallel private(thread)
     
           {
          thread = omp_get_thread_num();
           #pragma omp for
    for (int fila = 1; fila < hoja_sala.size(); fila++){

        string eSala = hoja_sala.at(fila).at(0);
        char cstr[eSala.size() + 1];
	      strcpy(cstr, eSala.c_str());
        strcpy(salass[fila].nombreEdificio, cstr);// nombre edificio

        string numSala = hoja_sala.at(fila).at(1);
        char num[numSala.size() + 1];
	    strcpy(num, numSala.c_str());
        strcpy(salass[fila].numeroSala, num);// nombre sala
        
        string completSala = eSala + "-" + numSala;
        char comSala[completSala.size() + 1];
        strcpy(comSala, completSala.c_str());
        strcpy(salass[fila].sala, comSala);

        esLab=hoja_sala.at(fila).at(0);
        if (lab== esLab){
            salass[fila].lab = 1;//Si es lab
        }
        if (lab!= esLab){
            salass[fila].lab = 0;//No es lab
        }
    }
           }
}

Horario block[54];
void asignarHorario(){
    int cantINF=0;
    int cantProf=0;
    int dia = 0;

    //Se vacia el horario
    for(int sal=1;sal<54;sal++){
        for (int dia=0;dia<6;dia++){
            for (int bloque=0;bloque<7;bloque++){
                block[sal].bloque[bloque][dia]="0";
                block[sal].restBlock[dia]=4;
            }
        }
    }

    for(int sal=1;sal<54;sal++){
            string queSala(salass[sal].sala);
            char qSala[queSala.size() + 1];
            strcpy(qSala, queSala.c_str());
            strcpy(block[sal].sala, qSala);
            //cout << "Sala: " << queSala << endl;
            //Comprueba si la sala es un lab (se asignan primero las clases en los labs)
            if (queSala=="LAB-1" || queSala=="LAB-2" || queSala=="LAB-3" || queSala=="LAB-4" || queSala=="LAB-5" || queSala=="LAB-6"){
                //cout << "Sala: " << queSala << endl;
                for (int c=1; c<347;c++){
                    //Se asignan primero los ramos que tengan 6
                    if(ramoss[c].codigoRamo[0]=='I' && ramoss[c].codigoRamo[1]=='N' && ramoss[c].codigoRamo[2]=='F' && ramoss[c].horasRamo==6){
                        //cout << "Codigo INF" << endl;
                        for(int z = 0; z<239;z++){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl;
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl; 
                            for (int dia=0;dia<6;dia++){
                                for (int bloque=0;bloque<7;bloque++){
                                    if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                        //juntar codProfe y codRamo
                                        string codCurso(ramoss[c].codigoRamo);
                                        std::stringstream ss;
                                        ss << profes[z].codigoProfesor;
                                        string codProfee = ss.str();
                                        //Se guarda segun estructura solicitada
                                        string codProCur = codCurso + "-" + codProfee;
                                        //cout << codProCur <<endl;
                                        block[sal].bloque[bloque][dia]=codProCur;
                                        //std::cout << "Horario: " << bloque << dia << ": " << block[sal].bloque[bloque][dia] << endl;
                                        //block[sal].bloque[bloque][dia]=profes[z].codigoProfesor;
                                        profes[z].diasBloques[bloque][dia] = 1;
                                        ramoss[c].horasRamo = ramoss[c].horasRamo -2;
                                        cantINF++;
                                        block[sal].restBlock[dia]--;
                                        if(ramoss[c].horasRamo<=0){
                                            ramoss[c].estado=1;
                                        }
                                    }
                                }
                            }
                            }
                        }
                    } else if (ramoss[c].codigoRamo[0]=='I' && ramoss[c].codigoRamo[1]=='N' && ramoss[c].codigoRamo[2]=='F' && ramoss[c].horasRamo==1){
                        //cout << "Sala: " << queSala << endl;
                        for(int z = 0; z<239;z++){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl;
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl; 
                            for (int dia=0;dia<6;dia++){
                                for (int bloque=0;bloque<7;bloque++){
                                    if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                        //juntar codProfe y codRamo
                                        string codCurso(ramoss[c].codigoRamo);
                                        std::stringstream ss;
                                        ss << profes[z].codigoProfesor;
                                        string codProfee = ss.str();
                                        //Se guarda segun estructura solicitada
                                        string codProCur = codCurso + "-" + codProfee;
                                        //cout << codProCur <<endl;
                                        block[sal].bloque[bloque][dia]=codProCur;
                                        profes[z].diasBloques[bloque][dia] = 1;
                                        ramoss[c].horasRamo = ramoss[c].horasRamo -1;
                                        cantINF++;
                                        block[sal].restBlock[dia]--;
                                        if(ramoss[c].horasRamo<=0){
                                            ramoss[c].estado=1;
                                        }
                                    }
                                }
			      }
                            }
                        }                        
                    }
                }
            } else { // FUNCION PARA LAS SALAS QUE NO SON LABS
                //cout << "Sala: " << queSala << endl;
                for (int c=1; c<347;c++){
                    //Se comprueba que no sea un lab
                    if((ramoss[c].codigoRamo[0]!='I' || ramoss[c].codigoRamo[1]!='N' || ramoss[c].codigoRamo[2]!='F') && ramoss[c].horasRamo==6){
                        //cout << "Codigo INF" << endl;
                        for(int z = 0; z<239;z++){
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl; 
                            for (int dia=0;dia<6;dia++){
                                for (int bloque=0;bloque<7;bloque++){
                                    if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                        //juntar codProfe y codRamo
                                        string codCurso(ramoss[c].codigoRamo);
                                        std::stringstream ss;
                                        ss << profes[z].codigoProfesor;
                                        string codProfee = ss.str();
                                        //Se guarda segun estructura solicitada
                                        string codProCur = codCurso + "-" + codProfee;
                                        //cout << codProCur <<endl;
                                        block[sal].bloque[bloque][dia]=codProCur;
                                        //std::cout << "Horario: " << bloque << dia << ": " << block[sal].bloque[bloque][dia] << endl;
                                        profes[z].diasBloques[bloque][dia] = 1;
                                        ramoss[c].horasRamo = ramoss[c].horasRamo -2;
                                        cantINF++;
                                        block[sal].restBlock[dia]--;
                                        if(ramoss[c].horasRamo<=0){
                                            ramoss[c].estado=1;
                                        }
                                    }
                                }}
                            }
                        }
                    } else if ((ramoss[c].codigoRamo[0]!='I' || ramoss[c].codigoRamo[1]!='N' || ramoss[c].codigoRamo[2]!='F') && ramoss[c].horasRamo==4){
                        //cout << "Sala: " << queSala << endl;
                        for(int z = 0; z<239;z++){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl;
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl; 
                            for (int dia=0;dia<6;dia++){
                                for (int bloque=0;bloque<7;bloque++){
                                    if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                        //juntar codProfe y codRamo
                                        string codCurso(ramoss[c].codigoRamo);
                                        std::stringstream ss;
                                        ss << profes[z].codigoProfesor;
                                        string codProfee = ss.str();
                                        //Se guarda segun estructura solicitada
                                        string codProCur = codCurso + "-" + codProfee;
                                        //cout << codProCur <<endl;
                                        block[sal].bloque[bloque][dia]=codProCur;
                                        //std::cout << "Horario: " << bloque << dia << ": " << block[sal].bloque[bloque][dia] << endl;
                                        //block[sal].bloque[bloque][dia]=profes[z].codigoProfesor;
                                        profes[z].diasBloques[bloque][dia] = 1;
                                        ramoss[c].horasRamo = ramoss[c].horasRamo -2;
                                        cantINF++;
                                        block[sal].restBlock[dia]--;
                                        if(ramoss[c].horasRamo<=0){
                                            ramoss[c].estado=1;
                                        }
                                    }}
                                }
                            }
                        }
                        
                    } else if ((ramoss[c].codigoRamo[0]!='I' || ramoss[c].codigoRamo[1]!='N' || ramoss[c].codigoRamo[2]!='F') && ramoss[c].horasRamo==1){
                        //cout << "Sala: " << queSala << endl;
                        for(int z = 0; z<239;z++){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl;
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                            //std::cout << "CodProfe: " << profes[z].codigoProfesor << endl; 
                            for (int dia=0;dia<6;dia++){
                                for (int bloque=0;bloque<7;bloque++){
                                    if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                        //juntar codProfe y codRamo
                                        string codCurso(ramoss[c].codigoRamo);
                                        std::stringstream ss;
                                        ss << profes[z].codigoProfesor;
                                        string codProfee = ss.str();
                                        //Se guarda segun estructura solicitada
                                        string codProCur = codCurso + "-" + codProfee;
                                        //cout << codProCur <<endl;
                                        block[sal].bloque[bloque][dia]=codProCur;
                                        //std::cout << "Horario: " << bloque << dia << ": " << block[sal].bloque[bloque][dia] << endl;
                                        profes[z].diasBloques[bloque][dia] = 1;
                                        ramoss[c].horasRamo = ramoss[c].horasRamo -1;
                                        cantINF++;
                                        block[sal].restBlock[dia]--;
                                        if(ramoss[c].horasRamo<=0){
                                            ramoss[c].estado=1;
                                        }
                                    }
                                }}
                            }
                        }
                    }
                }
            }
        //}
    }

/***********************************************************/
    //Ramos no asignados en el primer algoritmo
    for(int c=1; c<347;c++){
        if(ramoss[c].estado==0){
            if(ramoss[c].codigoRamo[0]=='I' && ramoss[c].codigoRamo[1]=='N' && ramoss[c].codigoRamo[2]=='F' && ramoss[c].horasRamo>0){
                for(int sal=53;sal>0;sal--){
                string queSala(salass[sal].sala);
                char qSala[queSala.size() + 1];
                strcpy(qSala, queSala.c_str());
                strcpy(block[sal].sala, qSala);
                        for(int z = 0; z<239;z++){
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                        
                        for (int dia=0;dia<6;dia++){
                            for (int bloque=0;bloque<7;bloque++){
                                if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                    //juntar codProfe y codRamo
                                    string codCurso(ramoss[c].codigoRamo);
                                    std::stringstream ss;
                                    ss << profes[z].codigoProfesor;
                                    string codProfee = ss.str();
                                    //Se guarda segun estructura solicitada
                                    string codProCur = codCurso + "-" + codProfee;
                                    //cout << codProCur <<endl;
                                    block[sal].bloque[bloque][dia]=codProCur;
                                    profes[z].diasBloques[bloque][dia] = 1;
                                    ramoss[c].horasRamo = ramoss[c].horasRamo -2;
                                    cantINF++;
                                    block[sal].restBlock[dia]--;
                                    if(ramoss[c].horasRamo<=0){
                                        ramoss[c].estado=1;
                                    }
                                }
                            }
                        }

                            }}  
                }
            } else if(ramoss[c].horasRamo>0){
                for(int sal=53;sal>0;sal--){
                string queSala(salass[sal].sala);
                char qSala[queSala.size() + 1];
                strcpy(qSala, queSala.c_str());
                strcpy(block[sal].sala, qSala);
                        for(int z = 0; z<239;z++){
                            if(ramoss[c].codigoProfesor == profes[z].codigoProfesor){
                        
                        for (int dia=0;dia<6;dia++){
                            for (int bloque=0;bloque<7;bloque++){
                                if(profes[z].diasBloques[bloque][dia] == 0 && block[sal].bloque[bloque][dia]=="0"  && (ramoss[c].horasRamo>0 && block[sal].restBlock[dia]>0)){
                                    //juntar codProfe y codRamo
                                    string codCurso(ramoss[c].codigoRamo);
                                    std::stringstream ss;
                                    ss << profes[z].codigoProfesor;
                                    string codProfee = ss.str();
                                    //Se guarda segun estructura solicitada
                                    string codProCur = codCurso + "-" + codProfee;
                                    //cout << codProCur <<endl;
                                    block[sal].bloque[bloque][dia]=codProCur;
                                    profes[z].diasBloques[bloque][dia] = 1;
                                    ramoss[c].horasRamo = ramoss[c].horasRamo -2;
                                    cantINF++;
                                    block[sal].restBlock[dia]--;
                                    if(ramoss[c].horasRamo<=0){
                                        ramoss[c].estado=1;
                                    }
                                }
                            }
                        }

                            }}
                }
            }
        }
    }


    int cant=0;
    int noCant=0;
    std::cout << "Cantidad de bloques asignados: " <<  cantINF << endl;
    int thread; //inicializacion variable para diferenciar hilo mientra se paraleliza
    #pragma omp parallel private(thread)
     
           {
          thread = omp_get_thread_num();
           #pragma omp for
           
    for (int c=1; c<347;c++){
        if(ramoss[c].estado==1){
            cant++;
        } else if(ramoss[c].estado==0){
            noCant++;
        }
    }}
    std::cout << "Cantidad de RAMOS asignados: " <<  cant << endl;
    std::cout << "Cantidad de RAMOS NO asignados: " <<  noCant << endl;
}

// FUNCION PARA ESCRITURA Y SALIDA DE ARCHIVO

void crear_hojas(){


    char buffer[500]; //Variable para almacenar nombre de edificio y hoja convertidos a char
    lxw_workbook  *workbook  = workbook_new("Horario.xlsx"); //Crear libro de trabajo con el nombre "Horario"

    for (int i = 1; i<54; i++){ // 54 es la cantidas de salas
        lxw_worksheet *worksheet = workbook_add_worksheet(workbook, block[i].sala); //Escribe el nombre en cada hoja

        // Escribir bloques en cada hoja de sala
        string cadena_bloques[8] = {"Bloques","08:00-09:30","09:40-11:10","11:20-12:50","13:00-14:30","14:40-16:10","16:20-17:50","18:00-19:30"};
        char buffer_bloques[500]; //Variable para almacenar los bloques convertidos a char
    	    for (int c = 0; c<8; c++){ // Recorre cadena de bloques
            strcpy(buffer_bloques,cadena_bloques[c].c_str()); //Convierte la cadena de String a char
      	    worksheet_write_string(worksheet,c, 0, buffer_bloques, NULL); //Escribe los bloques en la columa 0 de la hoja
      	    worksheet_set_column(worksheet, c, 0, 12, NULL); //Ajustar ancho de la columna de bloques
    	    }

           // Escribir dias del horario en cada hoja de sala
    	   string cadena[7] = {"Bloques","Lunes","Martes","Miércoles","Jueves","Viernes","Sábado"}; //Cadena con los dias del horario
    	   char buffer_cadena[500]; //Variable para almacenar los dias convertidos a char
    	   for (int p = 0; p<7; p++){ //Recorre cadena con los dias
     	       //cout << cadena[p] << endl;	
      	       strcpy(buffer_cadena,cadena[p].c_str()); //Convierte la cadena de String a char
      	       worksheet_write_string(worksheet, 0, p, buffer_cadena, NULL); //Escribe los dias en la fila 0 de hoja
    	   }

        //Escribir bloques en el horario
        for(int f=0; f<7; f++){//Bloques
            for(int j=0; j<6; j++){//Dias
                char buffer_bloque[500];
                strcpy(buffer_bloque,block[i].bloque[f][j].c_str());
                worksheet_write_string(worksheet, f+1, j+1, buffer_bloque, NULL);
            }
        }
    }         

    workbook_close(workbook);
}


/*********************************************************/

int main( int argc, char *argv[])
{
    //argc: numero total de argumentos pasados por linea de comandos
    //argv: array con los argumentos pasados por linea de comandos

    //Variable para guardar el argumento
    string argumento;

    
    //Leyendo los argumentos por argumentos
    for (int i = 0; i < argc; i++)
    {
	argumento = argv[i];
	//Si argumento es igual a -s, se utiliza Salas.xlsx
	if (argumento == "-s")
        {
            cout << "Funcion de Info de salas " << endl;
            infoSalas(argv[i + 1]);
        }
	//Si argumento es igual a -d, se utiliza Docentes.xlsx
        else if (argumento == "-d")
        {
            cout << "Funcion de Info de profe " << endl;
            infoProfe(argv[i +1]);
        }
	//Si argumento es igual a -c, se utiliza Cursos.xlsx
	else if(argumento == "-c")
        {
            cout << "Funcion de Info de ramos " << endl;
            infoRamos(argv[i +1]);
        }
    }
    asignarHorario();
    crear_hojas();
    return 0;
}

#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;


/**
 * @brief Actualiza la matriz resultado con el terreno que se está viendo
 * 		  en ese momento
 * 
 * @pre El agente debe estar orientado, es decir, su estado debe representar
 * 		su posición real.
 * 
 * @param terreno Terreno visible por los sensores del agente
 * @param st Estado del agente
 * @param matriz Matriz donde se almacena lo que se ve
 */
void PonerTerrenoEnMatriz (const vector<unsigned char> & terreno, 
		const state & st, vector< vector<unsigned char> > & matriz);


/**
 * @brief Selecciona un giro a realizar
 * 
 * @param giro_derecha Variable que indica si se gira a la derecha
 * @param giro_grande Variable que indica si el giro es grande
 * @return Action a realizar
 */
Action Girar (bool & giro_derecha, bool & giro_grande);


/**
 * @brief Marca la casilla en la que se encuentra el agente como visitada.
 * 		  Se usa en conjunto con la función
 * 
 * @pre El agente debe estar orientado, es decir, su estado debe representar
 * 		su posición real.
 * 
 * @param st Estado del agente 
 * @param matriz Matriz donde se guardan las casillas visitadas
 */
void ActualizarMapaVisitado (const state & st, vector<vector<int>> & matriz);


/**
 * @brief Permite saber cuántas veces has visitado una casilla.
 * 
 * @pre El agente debe estar orientado, es decir, su estado debe representar
 * 		su posición real.
 * 
 * @param st Estado del agente 
 * @param matriz Matriz donde se guardan las casillas visitadas
 * 
 * @return El número de veces que se ha visitado la casilla.
 * 		   Si el agente no está orientado entonces devolverá 0
 */
int VecesVisitadas (const state & st, 
				 const vector<vector<int>> & matriz);


/**
 * @brief Permite saber si hay una casilla del tipo pasado como parámetro
 * 		  en el campo de visión del agente.
 * 
 * @param tipo Tipo de casilla a buscar
 * @param terreno Campo de visión del agente
 * @return -1 Si no hay una casilla de ese tipo en su campo de visión @n
 * 			 0 Si hay una casilla y hay que seguir recto @n
 * 			 1 Si hay una casilla y hay que girar a la izquierda @n
 * 			 2 Si hay una casilla y hay que girar a la derecha @n
 */
int VeoUnaCasilla (const unsigned char & tipo, 
	const vector<unsigned char> & terreno);



Action ComportamientoJugador::think(Sensores sensores){

	Action accion = actIDLE;
	int a;

	// Primera vez que entra en think
	if (ultima_casilla == '\0') {
		// Poner los precipicios de los bordes en mapaResultado
		for (int i = 0; i < mapaResultado.size(); i++)
			for (int j = 0; j < mapaResultado[i].size(); j++)
				if ((i < 3) || (i > (mapaResultado.size() - 4)))
					mapaResultado[i][j] = 'P';
				else if ((j < 3) || (j > (mapaResultado[i].size()-4)))
					mapaResultado[i][j] = 'P';

		// Si los sensores funcionan bien (!= -1), está bien situado
		if (sensores.posC != -1 && sensores.posF != -1) {
			bien_situado = true;
			estado_actual.fil = sensores.posF;
			estado_actual.col = sensores.posC;
			estado_actual.brujula = sensores.sentido;
		}
	}

	// Actualización de las variables de estado
	switch (ultima_accion)
	{
		case actFORWARD:
			switch (estado_actual.brujula)
			{
			case norte: estado_actual.fil--; break;
			case noreste: estado_actual.fil--; estado_actual.col++; break;
			case este: estado_actual.col++; break;
			case sureste: estado_actual.fil++; estado_actual.col++; break;
			case sur: estado_actual.fil++; break;
			case suroeste: estado_actual.fil++; estado_actual.col--; break;
			case oeste: estado_actual.col--; break;
			case noroeste: estado_actual.fil--; estado_actual.col--; break;
			}
			veces_giradas = 0;
			break;
		case actTURN_SL:
			a = estado_actual.brujula;
			a = (a+7)%8;
			estado_actual.brujula = static_cast<Orientacion>(a);
			veces_giradas++;
			break;
		case actTURN_SR:
			a = estado_actual.brujula;
			a = (a+1)%8;
			estado_actual.brujula = static_cast<Orientacion>(a);
			veces_giradas++;
			break;
		case actTURN_BL:
			a = estado_actual.brujula;
			a = (a+5)%8;
			estado_actual.brujula = static_cast<Orientacion>(a);
			veces_giradas++;
			break;
		case actTURN_BR:
			a = estado_actual.brujula;
			a = (a+3)%8;
			estado_actual.brujula = static_cast<Orientacion>(a);
			veces_giradas++;
			break;
	}

	// Actualización mapaVisitado
	if (ultima_accion == actFORWARD && bien_situado) {
		ActualizarMapaVisitado(estado_actual, mapaVisitado);
	}

	// Actualización reinicio
	if (sensores.reset == true) {
		bien_situado = false;
		estado_actual.col = estado_actual.fil = -1;
		tengoBikini = tengoZapatillas = false;
	}

	// Actualización posición
	if (sensores.terreno[0] == 'G' && !bien_situado) {
		estado_actual.fil = sensores.posF;
		estado_actual.col = sensores.posC;
		estado_actual.brujula = sensores.sentido;
		bien_situado = true;
	}

	// Colocar terreno visible en mapaResultado
	if (bien_situado) {
		PonerTerrenoEnMatriz(sensores.terreno, estado_actual, mapaResultado);
	}

	// Actualización bikini y zapatillas
	if (sensores.terreno[0] == 'K' && !tengoBikini)
		tengoBikini = true;
	else if (sensores.terreno[0] == 'D' && !tengoZapatillas)
		tengoZapatillas = true;


	// Decisión
	if ((sensores.terreno[2] == 'T' || sensores.terreno[2] == 'S' 
		|| sensores.terreno[2] == 'G' || sensores.terreno[2] == 'K'
		|| sensores.terreno[2] == 'D' || sensores.terreno[2] == 'X'
		|| (sensores.terreno[2] == 'A' && tengoBikini)
		|| (sensores.terreno[2] == 'B' && tengoZapatillas)) 
		&& sensores.superficie[2] == '_') {
		accion = actFORWARD;
	}
	else if (veces_giradas < 5) {
		accion = Girar(girar_derecha, giro_grande);
	}
	else {
		if (sensores.terreno[2] != 'P' && sensores.terreno[2] != 'M' 
			&& sensores.superficie[2] == '_')
			accion = actFORWARD;
		else {
			bool tmp = true;
			accion = Girar(girar_derecha, tmp);
		}
	}


	// Aleatorizar los giros (hacerlo si la casilla ya está visitada)
	if (accion == actFORWARD && bien_situado
		&& VecesVisitadas(estado_actual, mapaVisitado) > 0) {
		if (VecesVisitadas(estado_actual, mapaVisitado) > 2 
			&& rand() % 3 == 0)
			accion = Girar(girar_derecha, giro_grande);
		else if (rand() % 10 == 0)
			accion = Girar(girar_derecha, giro_grande);
	}
	
	// No hacer giros pequeños si está en el agua y no tiene bikini
	if (sensores.terreno[0] == 'A' && !tengoBikini) {
		if (accion == actTURN_SL)
			accion = actTURN_BL;
		else if (accion == actTURN_SR)
			accion = actTURN_BR;
	}

	// Si ve una casilla bateria y le queda poca, ir hacia ella
	if (sensores.bateria < 1000) {
		switch (VeoUnaCasilla('X', sensores.terreno))
		{
			case 0: accion = actFORWARD; break;
			case 1: accion = actTURN_SL; break;
			case 2: accion = actTURN_SR; break;
		}
	}
	
	// Si no tiene zapatillas y ve la casilla, ir hacia ella
	if (!tengoZapatillas) {
		switch (VeoUnaCasilla('D', sensores.terreno))
		{
			case 0: accion = actFORWARD; break;
			case 1: accion = actTURN_SL; break;
			case 2: accion = actTURN_SR; break;
		}
	}

	// Si no tiene bikini y ve la casilla, ir hacia ella
	if (!tengoBikini) {
		switch (VeoUnaCasilla('K', sensores.terreno))
		{
			case 0: accion = actFORWARD; break;
			case 1: accion = actTURN_SL; break;
			case 2: accion = actTURN_SR; break;
		}
	}
	
	// Si no está orientado y ve casilla de posicionamiento, ir hacia ella
	if (!bien_situado) {
		switch (VeoUnaCasilla('G', sensores.terreno))
		{
			case 0: accion = actFORWARD; break;
			case 1: accion = actTURN_SL; break;
			case 2: accion = actTURN_SR; break;
		}
	}

	// Si está en casilla de batería y tiene poca batería, recargarla
	if (sensores.bateria < 1500 && sensores.terreno[0] == 'X')
		accion = actIDLE;
	

	// Si se intenta avanzar pero no se puede, girar
	if (accion == actFORWARD && (sensores.superficie[2] != '_' 
		|| sensores.terreno[2] == 'P' || sensores.terreno[2] == 'M'))
		accion = Girar(girar_derecha, giro_grande);


	ultima_accion = accion;
	ultima_casilla = sensores.terreno[0];

	// Determinar el efecto de la ultima accion enviada
	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}


typedef pair<int,int> p;

// Dirección norte
vector<pair<int, int>> aux1 = {
	p(-1,-1), p(-1, 0), p(-1, 1), p(-2,-2), p(-2,-1),
	p(-2, 0), p(-2, 1), p(-2, 2), p(-3,-3), p(-3,-2),
	p(-3,-1), p(-3, 0), p(-3, 1), p(-3, 2), p(-3, 3)
};

// Dirección noroeste
vector<pair<int,int>> aux2 = {
	p( 0,-1), p(-1,-1), p(-1, 0), p( 0,-2), p(-1,-2), 
	p(-2,-2), p(-2,-1), p(-2, 0), p( 0,-3), p(-1,-3), 
	p(-2,-3), p(-3,-3), p(-3,-2), p(-3,-1), p(-3, 0)
};


void PonerTerrenoEnMatriz (const vector<unsigned char> & terreno, 
		const state & st, vector< vector<unsigned char> > & matriz) {

	matriz[st.fil][st.col] = terreno[0];

	switch (st.brujula)
	{
		case norte:
			for (int i = 1; i < 16; i++)
				matriz[st.fil+aux1[i-1].first][st.col+aux1[i-1].second] = terreno[i];	
			break;
		case noreste:
			for (int i = 1; i < 16; i++)
				matriz[st.fil+aux2[i-1].second][st.col-aux2[i-1].first] = terreno[i];
			break;
		case este:
			for (int i = 1; i < 16; i++)
				matriz[st.fil+aux1[i-1].second][st.col-aux1[i-1].first] = terreno[i];
			break;
		case sureste:
			for (int i = 1; i < 16; i++)
				matriz[st.fil-aux2[i-1].first][st.col-aux2[i-1].second] = terreno[i];
			break;
		case sur:
			for (int i = 1; i < 16; i++)
				matriz[st.fil-aux1[i-1].first][st.col-aux1[i-1].second] = terreno[i];		
			break;
		case suroeste:
			for (int i = 1; i < 16; i++)
				matriz[st.fil-aux2[i-1].second][st.col+aux2[i-1].first] = terreno[i];
			break;
		case oeste:
			for (int i = 1; i < 16; i++)
				matriz[st.fil-aux1[i-1].second][st.col+aux1[i-1].first] = terreno[i];
			break;
		case noroeste:	
			for (int i = 1; i < 16; i++)
				matriz[st.fil+aux2[i-1].first][st.col+aux2[i-1].second] = terreno[i];
			break;
	}
}


Action Girar (bool & girar_derecha, bool & giro_grande) {
	Action accion;

	if (girar_derecha) {
		if (giro_grande)
			accion = actTURN_BR;
		else
			accion = actTURN_SR;
	}
	else {
		if (giro_grande)
			accion = actTURN_BL;
		else
			accion = actTURN_SL;
	}

	girar_derecha = (rand() % 2 == 0);
	giro_grande = (rand() % 2 == 0);

	return accion;
}


void ActualizarMapaVisitado (const state & st, vector<vector<int>> & matriz) {
	matriz[st.fil][st.col]++;
}


int VecesVisitadas (const state & st, 
		const vector<vector<int>> & matriz) {
	// No debería ocurrir nunca este caso
	if (st.fil == -1 && st.col == -1)
		return 0;
	
	int x = st.fil;
	int y = st.col;

	switch (st.brujula)
	{
		case norte: x--; break;
		case noreste: x--; y++; break;
		case este: y++; break;
		case sureste: x++; y++; break;
		case sur: x++; break;
		case suroeste: x++; y--; break;
		case oeste: y--; break;
		case noroeste: x--; y--; break;
	}

	return matriz[x][y];
}


int VeoUnaCasilla (const unsigned char & tipo,
	const vector<unsigned char> & terreno) {
	int c = -1;

	for (int i = 0; i < terreno.size() && (c == -1); i++)
		if (terreno[i] == tipo)
			c = i;
	
	// Ha encontrado la c en su campo de visión
	if (c > -1) {
		if (c == 1 || c == 4 || c == 9)
			c = 1;
		else if (c == 3 || c == 8 || c == 15)
			c = 2;
		else
			c = 0;
	}

	return c;
}
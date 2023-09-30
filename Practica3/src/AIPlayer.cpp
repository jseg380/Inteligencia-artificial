# include "AIPlayer.h"
# include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    // El siguiente código se proporciona como sugerencia para iniciar la implementación del agente.

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta
  
    // Si quiero poder manejar varias heurísticas, puedo usar la variable id del agente para usar una u otra.
    switch(id){
        case 0:
            // ID 0: Reservado para ValoracionTest
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
            break;
        case 1:
            // ID 1: Mejor heurística
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion2);
            break;
        case 2:
            valor = Poda_AlfaBeta(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, MiValoracion1);
            break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}



double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.


    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}


double AIPlayer::Poda_AlfaBeta(const Parchis &actual, int jugador, int profundidad, 
                               int profundidad_max, color &c_piece, int &id_piece, 
                               int &dice, double alpha, double beta, 
                               double (*heuristic)(const Parchis &, int)) const
{
    // Casos base
    if (actual.gameOver() && actual.getWinner() == jugador)
        return gana;
    else if (actual.gameOver())
        return pierde;
    else if (profundidad == profundidad_max)
        return heuristic(actual, jugador);
    // Caso general
    else {
        color color_pieza = c_piece;
        int pieza = id_piece;
        int dado = dice;
        double puntuacion;

        ParchisBros hijos = actual.getChildren();

        // Max
        if (jugador == actual.getCurrentPlayerId()) {
            for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() 
                 && !(alpha >= beta); ++it) {

                puntuacion = Poda_AlfaBeta(*it, jugador, profundidad+1, 
                                           profundidad_max, c_piece, id_piece, 
                                           dice, alpha, beta, *heuristic);

                if (alpha < puntuacion) {
                    alpha = puntuacion;
                    if (profundidad == 0) {
                        color_pieza = it.getMovedColor();
                        pieza = it.getMovedPieceId();
                        dado = it.getMovedDiceValue();
                    }
                }
            }

            // Actualizar información sobre la pieza que se debe mover
            c_piece = color_pieza;
            id_piece = pieza;
            dice = dado;
            return alpha;
        }
        // Min
        else {
            for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() 
                 && !(alpha >= beta); ++it) {

                puntuacion = Poda_AlfaBeta(*it, jugador, profundidad+1, 
                                           profundidad_max, c_piece, id_piece, 
                                           dice, alpha, beta, *heuristic);

                if (beta > puntuacion) {
                    beta = puntuacion;
                    if (profundidad == 0) {
                        color_pieza = it.getMovedColor();
                        pieza = it.getMovedPieceId();
                        dado = it.getMovedDiceValue();
                    }
                }
            }

            // Actualizar información sobre la pieza que se debe mover
            c_piece = color_pieza;
            id_piece = pieza;
            dice = dado;
            return beta;
        }
    }
}


double AIPlayer::MiValoracion1(const Parchis &estado, int jugador) {

    int puntuacion = 0;

    // Parte específica
    
    if (estado.isStarMove())
        puntuacion += 35;
    else if (estado.isBulletMove())
        puntuacion += 30;
    else if (estado.isBooMove())
        puntuacion += 25;
    else if (estado.isMushroomMove() || estado.isMegaMushroomMove())
        puntuacion += 10;

    // Parte básica, común al juego del parchís
    // Estrategias básicas sin tener en cuenta los dados especiales
    
    // Jugador
    vector<color> colores_jugador = estado.getPlayerColors(jugador);
    
    for (int i = 0; i < colores_jugador.size(); i++) {
        color c = colores_jugador[i];

        for (int p = 0; p < num_pieces; p++) {

            int piezas_en_casa = estado.piecesAtHome(c);
            
            // Cuantas más piezas tenga en casa el jugador, peor
            puntuacion -= piezas_en_casa * 20;

            // Se recompensa cuando las fichas del jugador están cerca de la meta
            puntuacion += (300 - (estado.distanceToGoal(c, p) * 20));

            // Recompensar si el jugador se ha comido una ficha, sin importar de
            // quien sea, ya que por lo general son 20 casillas menos
            if (estado.isEatingMove())
                puntuacion += 120;
            // Si no ha comido ficha pero ha llegado a una casilla segura
            // recompensar menos
            else if (estado.isSafePiece(c, p))
                puntuacion += 20;
            else if (estado.getBoard().getPiece(c, p).get_box().type == goal) {
                // Recompensar al jugador por tener ficha(s) en la meta
                puntuacion += 200;

                // Si con al menos una ficha en la meta hay alguna ficha
                // en la casa penalizar aún más
                puntuacion -= piezas_en_casa * 20;
            }
            // Recompensar si ha creado una barrera para entorpecer al oponente
            else if (c == estado.isWall(estado.getBoard().getPiece(c, p).get_box()))
                puntuacion += 6;
        }
    }
    
    // Oponente
    int oponente = (jugador+1)%2;
    vector<color> colores_oponente = estado.getPlayerColors(oponente);

    for (int i = 0; i < colores_oponente.size(); i++) {
        color c = colores_oponente[i];

        for (int p = 0; p < num_pieces; p++) {

            int piezas_en_casa = estado.piecesAtHome(c);

            // Cuantas más piezas tenga en casa el oponente, mejor
            puntuacion += piezas_en_casa * 20;

            // Se penaliza cuando las fichas opuestas están cerca de la meta
            puntuacion -= (300 - (estado.distanceToGoal(c, p) * 20));

            // Penalizar si el oponente se ha comido una ficha, sin importar de
            // quien sea, ya que por lo general son 20 casillas menos
            if (estado.isEatingMove())
                puntuacion -= 120;
            // Si no ha comido ficha pero ha llegado a una casilla segura
            // penalizar, porque la ficha no es vulnerable a ser comida
            else if (estado.isSafePiece(c, p))
                puntuacion -= 20;
            else if (estado.getBoard().getPiece(c, p).get_box().type == goal) {
                // Penalizar por tener el oponente ficha(s) en la meta
                puntuacion -= 200;

                // Si con al menos una ficha en la meta el oponente tiene 
                // alguna ficha en la casa recompensar aún más
                puntuacion += piezas_en_casa * 20;
            }
            // Penalizar si el oponente ha creado una barrera para entorpecer 
            // al jugador
            else if (c == estado.isWall(estado.getBoard().getPiece(c, p).get_box()))
                puntuacion -= 6;
            // Penalizar si el oponente ha llegado al pasillo final, porque
            // está muy cerca de la meta
            else if (estado.getBoard().getPiece(c, p).get_box().type == final_queue)
                puntuacion -= 100;
        }
    }

    return puntuacion;
}


double AIPlayer::MiValoracion2(const Parchis &estado, int jugador) {

    int puntuacion = 0;
    
    // Jugador
    vector<color> colores_jugador = estado.getPlayerColors(jugador);
    
    for (int i = 0; i < colores_jugador.size(); i++) {
        color c = colores_jugador[i];

        for (int p = 0; p < num_pieces; p++) {

            int piezas_en_casa = estado.piecesAtHome(c);
            
            // Cuantas más piezas tenga en casa el jugador, peor
            puntuacion -= piezas_en_casa * 20;

            // Se recompensa cuando las fichas del jugador están cerca de la meta
            puntuacion += (300 - (estado.distanceToGoal(c, p) * 20));

            // Recompensar si el jugador se ha comido una ficha, sin importar de
            // quien sea, ya que por lo general son 20 casillas menos
            if (estado.isEatingMove())
                puntuacion += 120;
            // Si no ha comido ficha pero ha llegado a una casilla segura
            // recompensar menos
            else if (estado.isSafePiece(c, p))
                puntuacion += 20;
            else if (estado.getBoard().getPiece(c, p).get_box().type == goal) {
                // Recompensar al jugador por tener ficha(s) en la meta
                puntuacion += 200;

                // Si con al menos una ficha en la meta hay alguna ficha
                // en la casa penalizar aún más
                if (piezas_en_casa > 0) {
                    if (piezas_en_casa == 1)
                        puntuacion -= 20;
                    else // 2 fichas en casa
                        puntuacion -= 40;
                }
            }
            // Recompensar si ha creado una barrera para entorpecer al oponente
            else if (c == estado.isWall(estado.getBoard().getPiece(c, p).get_box()))
                puntuacion += 6;
        }
    }
    
    // Oponente
    int oponente = (jugador+1)%2;
    vector<color> colores_oponente = estado.getPlayerColors(oponente);

    for (int i = 0; i < colores_oponente.size(); i++) {
        color c = colores_oponente[i];

        for (int p = 0; p < num_pieces; p++) {

            int piezas_en_casa = estado.piecesAtHome(c);

            // Cuantas más piezas tenga en casa el oponente, mejor
            puntuacion += piezas_en_casa * 20;

            // Se penaliza cuando las fichas opuestas están cerca de la meta
            puntuacion -= (300 - (estado.distanceToGoal(c, p) * 20));

            // Penalizar si el oponente se ha comido una ficha, sin importar de
            // quien sea, ya que por lo general son 20 casillas menos
            if (estado.isEatingMove())
                puntuacion -= 120;
            // Si no ha comido ficha pero ha llegado a una casilla segura
            // penalizar, porque la ficha no es vulnerable a ser comida
            else if (estado.isSafePiece(c, p))
                puntuacion -= 20;
            else if (estado.getBoard().getPiece(c, p).get_box().type == goal) {
                // Penalizar por tener el oponente ficha(s) en la meta
                puntuacion -= 200;

                // Si con al menos una ficha en la meta el oponente tiene 
                // alguna ficha en la casa recompensar aún más
                if (piezas_en_casa > 0) {
                    if (piezas_en_casa == 1)
                        puntuacion += 20;
                    else // 2 fichas en casa
                        puntuacion += 40;
                }
            }
            // Penalizar si el oponente ha creado una barrera para entorpecer 
            // al jugador
            else if (c == estado.isWall(estado.getBoard().getPiece(c, p).get_box()))
                puntuacion -= 6;
            // Penalizar si el oponente ha llegado al pasillo final, porque
            // está muy cerca de la meta
            else if (estado.getBoard().getPiece(c, p).get_box().type == final_queue)
                puntuacion -= 100;
        }
    }

    return puntuacion;
}

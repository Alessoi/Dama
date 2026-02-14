#include "player.hpp"

using namespace std;

struct Player::Impl{
    int player_nr;
    struct Cell;
    Cell* head;

    Impl();
    Impl(int num);
    ~Impl();

    void prepend(piece mat[8][8]);
    void append(piece mat[8][8]);
    void pop();
    void destroy();
    void stampa();
    bool compare_boards(piece mat[8][8]);
    void salvaSuFile(piece mat[8][8], const string& filename);
    int muovi(piece mat[8][8], int &pX1, int &pY1, int &xDir, int &yDir);
    pair<int, int> pezziRimasti(piece mat[8][8]);
    bool validaSingola(piece mat[8][8]);
    void copiaMatrice(piece dest[8][8], piece source[8][8]);
    void promuovi(piece mat[8][8]);
    string pString(piece p);
};

struct Player::Impl::Cell{
    Cell* next;
    piece board[8][8];
    Cell();
};

Player::Player(int player_nr){
    if(player_nr < 1 || player_nr > 2){
        throw player_exception{player_exception::index_out_of_bounds, "Player number errato"};
    }
    pimpl = new Impl(player_nr);
}

Player::~Player(){
    delete pimpl;
}

Player::Player(const Player& p){
    pimpl = new Impl();
    *this = p;
}

Player::Impl::Impl(){
    player_nr = 0;
    head = nullptr;
    srand(10);
}

Player::Impl::Impl(int num){
    player_nr = num;
    head = nullptr;
    srand(10);
}

Player::Impl::~Impl(){
    destroy();
}

void Player::Impl::prepend(piece mat[8][8]){
    Cell* nuova = new Cell();
    nuova->next = head;

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            nuova->board[i][j] = mat[i][j];
        }
    }

    head = nuova;
}

void Player::Impl::append(piece mat[8][8]){
    Cell* nuova = new Cell(), *temp=head;
    nuova->next = nullptr;
    
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            nuova->board[i][j] = mat[i][j];
        }
    }

    while(temp != nullptr && temp->next != nullptr){
        temp = temp->next;
    }

    if(temp){
        temp->next = nuova;
    }else{
        head = nuova;
    }
}

void Player::Impl::pop(){
    if(head != nullptr){
        Cell* temp = head->next;
        delete head;
        head = temp;
    }else{
        throw player_exception{player_exception::index_out_of_bounds, "History vuota"};
    }
}

void Player::Impl::destroy(){
    while(head != nullptr){
        Cell* next = head->next;
        delete head;
        head = next;
    }
    head = nullptr;
}

void Player::Impl::stampa(){
    Cell* temp = head;
    int c=0;
    cout << endl << endl;
    while(temp != nullptr){
        cout << "Round " << c << endl;
        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                cout << pString(temp->board[i][j]);
            }
            cout << endl;
        }
        cout << endl << endl;
        temp = temp->next;
        c++;
    }
    cout << endl << endl;
}

bool Player::Impl::compare_boards(piece mat[8][8]){
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(head->board[i][j] != mat[i][j]){
                return false;
            }
        }
    }
    return true;
}

void Player::Impl::salvaSuFile(piece mat[8][8], const string& filename){
    std::ofstream file(filename);
    for(int i=0; i<8; i++){
        int j=0;
        if(i%2 == 0){
            file << pString(mat[i][j]) << "   ";
            j=j+2;
            file << pString(mat[i][j]) << "   ";
            j=j+2;
            file << pString(mat[i][j]) << "   ";
            j=j+2;
            file << pString(mat[i][j]);
            file << "  ";
            j=j+2;
            
        }else{
            file << "  ";
            j=1;
            file << pString(mat[i][j]) << "   ";
            j=j+2;
            file << pString(mat[i][j]) << "   ";
            j=j+2;
            file << pString(mat[i][j]) << "   ";
            j=j+2;
            file << pString(mat[i][j]);
        }
        
        if(i < 7){
            file << std::endl;
        }
    }

    file.close();
}

Player::Impl::Cell::Cell(){
    next = nullptr;
}

Player& Player::operator=(const Player& p){
    Player::Impl::Cell* temp = p.pimpl->head;
    pimpl->player_nr = p.pimpl->player_nr;
    
    if(pimpl->head != nullptr){
        pimpl->destroy();
    }

    while(temp != nullptr){
        pimpl->append(temp->board);
        temp = temp->next;
    }

    return *this;
}

Player::piece Player::operator()(int r, int c, int history_offset) const{
    Player::Impl::Cell* temp =  pimpl->head;
    int i=0;
    
    if((r < 0 || r >= 8) || (c < 0 || c >= 8)){
        //errore
        throw player_exception{player_exception::index_out_of_bounds, "Questa riga o colonna non esiste"}; 
    }else if(history_offset < 0){
        throw player_exception{player_exception::index_out_of_bounds, "History offset non presente in memoria"}; 
    }

    while(i < history_offset && temp != nullptr){
        temp = temp->next;
        i++;
    }

    if(temp){
        return temp->board[r][c];
    }else{
        //errore
        throw player_exception{player_exception::index_out_of_bounds, "History offset non presente in memoria"};
    }
}

void Player::load_board(const string& filename){
    piece board[8][8];
    string line;
    int i=0;
    
    std::ifstream file(filename);
    if(!file.good()){
        // cout << "file no good";
        throw player_exception{player_exception::missing_file, "Errore sul file"};
    }

    while(getline(file, line)){
        if(i >= 8){
            // cout << "Troppe righe" << std::endl;
            throw player_exception{player_exception::invalid_board, "Troppe righe nella board"};
        }
        
        if(line.size() != 15){
            throw player_exception{player_exception::invalid_board, "Formato sbagliato board"};
        }
        int j=0, j2=0;

        if(i%2 == 0){
            j=1;
            while(j <= 9){
                if(!(line.at(j) == ' ' && line.at(j+1) == ' ' && line.at(j+2) == ' ')){
                    throw player_exception{player_exception::invalid_board, "Pedina in casella sbagliata"};
                }
                j=j+4;
            }
            j=13;
            if(!(line.at(j) == ' ' && line.at(j+1) == ' ')){
                throw player_exception{player_exception::invalid_board, "Pedina in casella sbagliata"};
            }
        }else{
            j=0;
            if(!(line.at(j) == ' ' && line.at(j+1) == ' ')){
                throw player_exception{player_exception::invalid_board, "Pedina in casella sbagliata"};
            }
            j=3;
            while(j<=11){
                if(!(line.at(j) == ' ' && line.at(j+1) == ' ' && line.at(j+2) == ' ')){
                    throw player_exception{player_exception::invalid_board, "Pedina in casella sbagliata"};
                }
                j=j+4;
            }
        }

        for(int j=0; j<15; j=j+2){
            switch (line.at(j)){
                case 'x': board[i][j2] = x;
                    break;
                case 'X': board[i][j2] = X;
                    break;
                case 'o': board[i][j2] = o;
                    break;
                case 'O': board[i][j2] = O;
                    break;
                case ' ': board[i][j2] = e;
                    break;
                default: 
                    throw player_exception{player_exception::invalid_board, "Carattere non autorizzato nella board"};
                    break;
            }
            j2++;
        }
        i++;
    }
    
    if(i != 8){
        throw player_exception{player_exception::invalid_board, "Errore righe nella board"};
    }

    file.close();

    if(!pimpl->validaSingola(board)){
        // cout << "non valida" << std::endl;
        throw player_exception{player_exception::invalid_board, "Board non valida"};
    }

    pimpl->prepend(board);
}

void Player::init_board(const string& filename) const{
    piece board[8][8];

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            board[i][j] = e;
        }
    }

    for(int i=0; i<8; i++){
        int offset = i%2;
        for(int j=0; j<8; j++){
            if(j % 2 == 0){
                if(i < 3){
                    board[i][j+offset] = o;
                }else if(i >= 5 ){
                    board[i][j+offset] = x;
                }
            }
        }
    }

    pimpl->salvaSuFile(board, filename);
}

void Player::store_board(const string& filename, int history_offset) const{
    Player::Impl::Cell* temp = pimpl->head;

    while(temp != nullptr && history_offset > 0){
        temp = temp->next;
        history_offset--;
    }

    if(temp != nullptr && history_offset == 0){
        pimpl->salvaSuFile(temp->board, filename);
    }else{
        //errore
        throw player_exception{player_exception::index_out_of_bounds, "History offset troppo grande"};
    }
}

void Player::move(){
    piece newBoard[8][8];
    int yDir = 0, xDir = 0, promotingRow = 0;
    piece myPieceS = x, myPieceB = X;

    if(pimpl->head == nullptr){
        throw player_exception{player_exception::invalid_board, "History vuota"};
    }

    if(pimpl->player_nr == 1){
        yDir = -1;
    }else{
        myPieceS = o;
        myPieceB = O;
        yDir = 1;
        promotingRow = 7;
    }

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            newBoard[i][j] = pimpl->head->board[i][j];
        }
    }

    int mossa = -1;
    int pX = 0, pY = 0;

    mossa = pimpl->muovi(newBoard, pX, pY, xDir, yDir);
    // std::cout << "m: " << mossa  << " px: " << pX << " xD: " << xDir << std::endl;

    if(mossa == 0){
        newBoard[pY+yDir][pX+xDir] = newBoard[pY][pX];
        newBoard[pY][pX] = e;
    }else if(mossa == 1){
        newBoard[pY+yDir*2][pX+xDir*2] = newBoard[pY][pX];
        newBoard[pY+yDir][pX+xDir] = e;
        newBoard[pY][pX] = e;
    }else if(mossa == 2){
        //niente
    }    

    for(int i=0; i<8; i++){
        if(newBoard[promotingRow][i] == myPieceS){
            newBoard[promotingRow][i] = myPieceB;
        }
    }

    pimpl->prepend(newBoard);
}

int Player::Impl::muovi(piece mat[8][8], int &pX1, int &pY1, int &xDir, int &yDir){
    int yD = 0, xD = 0, prove = 0, mossa = 2, pr1 = 1, pr2 = 8, pr3 = -1, pr4 = 6;
    piece myPieceS = x, myPieceB = X, ePieceS = x, ePieceB = X;

    if(player_nr == 1){
        ePieceS = o;
        ePieceB = O;
        yD = -1;
    }else{
        myPieceS = o;
        myPieceB = O;
        yD = 1;
        pr1 = -1;
        pr2 = 6;
        pr3 = 1;
        pr4 = 8;
    }

    bool done = false;
    int rx, ry, rm, rd;

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(mat[i][j] == myPieceS){
                if(j < 6 && (i < pr2 && i > pr1)  && mat[i + yD][j + 1] == ePieceS && mat[i + yD*2][j + 1*2] == e){
                    mossa = 1;
                    pX1 = j;
                    pY1 = i;
                    xDir = 1;
                    yDir = yD;
                    done = true;
                }else if(j > 1 && (i < pr2 && i > pr1 ) && mat[i + yD][j - 1] == ePieceS && mat[i + yD*2][j - 1*2] == e){
                    mossa = 1;
                    pX1 = j;
                    pY1 = i;
                    xDir = -1;
                    yDir = yD;
                    done = true;
                }
            }else if(mat[i][j] == myPieceB){
                if(j < 6 && (i < pr2 && i > pr1 )  && (mat[i + yD][j + 1] == ePieceS || mat[i + yD][j + 1] == ePieceB) && mat[i + yD*2][j + 1*2] == e){
                    mossa = 1;
                    pX1 = j;
                    pY1 = i;
                    xDir = 1;
                    yDir = yD;
                    done = true;
                }else if(j > 1 && (i < pr2 && i > pr1 ) && (mat[i + yD][j - 1] == ePieceS || mat[i + yD][j - 1] == ePieceB) && mat[i + yD*2][j - 1*2] == e){
                    mossa = 1;
                    pX1 = j;
                    pY1 = i;
                    xDir = -1;
                    yDir = yD;
                    done = true;
                }else if(j < 6 && (i < pr4 && i > pr3 ) && (mat[i - yD][j + 1] == ePieceS || mat[i - yD][j + 1] == ePieceB) && mat[i - yD*2][j + 1*2] == e){
                    mossa = 1;
                    pX1 = j;
                    pY1 = i;
                    xDir = 1;
                    yDir = -yD;
                    done = true;
                }else if(j > 1 && (i < pr4 && i > pr3 ) && (mat[i - yD][j - 1] == ePieceS || mat[i - yD][j - 1] == ePieceB) && mat[i - yD*2][j - 1*2] == e){
                    mossa = 1;
                    pX1 = j;
                    pY1 = i;
                    xDir = -1;
                    yDir = -yD;
                    done = true;
                }
            }

            if(done){
                return mossa;
            }
        }
    }

    while(!done && prove < 500){
        rx = rand()%8;
        ry = rand()%8;

        if(mat[ry][rx] == myPieceS){
            rm = rand()%2;
            bool ok = true;

            if(rm == 1){
                xD = -1;
                if(rx == 0) ok = false;
            }else{
                xD = 1;
                if(rx == 7) ok = false;
            }

            if(ok){
                if(mat[ry + yD][rx + xD] == e){
                    mossa = 0;
                    pX1 = rx;
                    pY1 = ry;
                    xDir = xD;
                    yDir = yD;
                    done = true;
                }
            }
        }else if(mat[ry][rx] == myPieceB){
            rm = rand()%2, rd = rand()%2;
            int yD2;
            bool ok = true;

            if(rm == 1){
                xD = -1;
                if(rx == 0) ok = false;
            }else{
                xD = 1;
                if(rx == 7) ok = false;
            }
            if(rd == 1){
                if(ry == 0) ok = false;
                yD2 = -1;
            }else{
                if(ry == 7) ok = false;
                yD2 = 1;
            }

            if(ok){
                if(mat[ry + yD2][rx + xD] == e){
                    mossa = 0;
                    pX1 = rx;
                    pY1 = ry;
                    xDir = xD;
                    yDir = yD2;
                    done = true;
                }
            }
        }
        prove++;
    }

    if(prove >= 500){
        mossa = 2;
    }

    return mossa;
}

void Player::Impl::copiaMatrice(piece dest[8][8], piece source[8][8]){
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            dest[i][j] = source[i][j];
        }
    }
}

void Player::Impl::promuovi(piece mat[8][8]){
    for(int i=0; i<8; i++){
        if(mat[0][i] == x){
            mat[0][i] = X;
        }
        if(mat[7][i] == o){
            mat[7][i] = O;
        }
    }
}

pair<int, int> Player::Impl::pezziRimasti(piece mat[8][8]){
    pair<int, int> out = {0, 0};

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(mat[i][j] == x || mat[i][j] == X){
                out.first++;
            }else if(mat[i][j] == o || mat[i][j] == O){
                out.second++;
            }
        }
    }

    return out;
}

bool Player::Impl::validaSingola(piece mat[8][8]){
    pair<int,int> rim = pezziRimasti(mat);
    if(rim.first > 12 || rim.second > 12){
        return false;
    }

    bool vuoto = false;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(vuoto && mat[i][j] != e){
                return false;
            }
            vuoto = !vuoto;
        }
        vuoto = !vuoto;
    }

    return true;
}

bool Player::valid_move() const{
    if(pimpl->head != nullptr && pimpl->head->next != nullptr){
        if(!pimpl->validaSingola(pimpl->head->board)){
            //cout << "NON VALIDA 1";
            return false;
        }

        piece board1[8][8], board2[8][8];

        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                board1[i][j] = pimpl->head->board[i][j];
                board2[i][j] = pimpl->head->next->board[i][j];
            }
        }

        if(pimpl->compare_boards(pimpl->head->next->board)){
            //cout << "UGUALI";
            return false;
        }
        
        int sub[8][8];
        int count = 0;

        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                int b1 = board1[i][j], b2 = board2[i][j];

                if(b1 == 4){
                    b1 = 0;
                }else{
                    b1++;
                }
                if(b2 == 4){
                    b2 = 0;
                }else{
                    b2++;
                }

                sub[i][j] =  b1 - b2;
            }
        }

        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                if(sub[i][j] != 0){
                    count++;
                }
            }
        }

        if(count > 0){
            pair<int, int> *pos = new pair<int, int>[count];
            piece *pOld = new piece[count], *pNew = new piece[count];
            int z = 0;
            bool out = true;

            for(int i=0; i<8; i++){
                for(int j=0; j<8; j++){
                    if(sub[i][j] != 0){
                        pos[z] = {j, i};
                        
                        if(sub[i][j] > 0){
                            if(board1[i][j] == sub[i][j]-1){
                                pOld[z] = e;
                                pNew[z] = board1[i][j];
                            }else{
                                //cout << "SOSTITUITO 1"  << " . " << board1[i][j] << " . " << sub[i][j]-1 << " ";
                                out = false;
                            }
                        }else if(sub[i][j] < 0){
                            if(board2[i][j] == -sub[i][j]-1){
                                pNew[z] = e;
                                pOld[z] = board2[i][j];
                            }else{
                                //cout << "SOSTITUITO 2" << " . " << board2[i][j] << " . " << -sub[i][j]-1 << " ";
                                out = false;
                            }
                        }

                        z++;
                    }
                }
            }

            if(count == 2 && out){
                if(pNew[0] == e && pNew[1] == e){
                    //cout << "VUOTI";
                    out = false;
                }else if(pNew[1] != e && pNew[0] == e){
                    piece temp = pNew[1];
                    pNew[1] = pNew[0];
                    pNew[0] = temp;
                    
                    temp = pOld[1];
                    pOld[1] = pOld[0];
                    pOld[0] = temp;

                    pair<int,int> temp1 = pos[1];
                    pos[1] = pos[0];
                    pos[0] = temp1;
                }
                
                int dirX = pos[1].first - pos[0].first, dirY = pos[1].second - pos[0].second;
                
                if(dirX != 1 && dirX != -1){
                    //cout << "TROPPO X1 " << dirX << " ";
                    out = false;
                }

                if(pNew[0] == x){
                    if(dirY != 1){
                        //cout << "TROPPO Y1 "  << dirY;
                        out = false;
                    }
                }if(pNew[0] == o){
                    if(dirY != -1){
                        //cout << "TROPPO Y2 " << dirY  << " " << pos[1].second << " " << pos[0].second << " ";
                        out = false;
                    }
                }else if(dirY != -1 && dirY != 1){
                    //cout << "TROPPO Y3";
                    out = false;
                }

                if(pOld[1] == x && out){
                    if(pos[0].second != 0){
                        if(pNew[0] != pOld[1]){
                            //cout << "DIVERSE 1";
                            out = false;
                        }
                    }else if(pNew[0] != X){
                        //cout << "NON PROMOSsA !";
                        out = false;
                    }
                }else if(pOld[1] == o){
                    if(pos[0].second != 7){
                        if(pNew[0] != pOld[1]){
                            //cout << "DIVERSE 2";
                            out = false;
                        }
                    }else if(pOld[1] == o && pNew[0] != O){
                        //cout << "NON PROMOSsA 2!";
                        out = false;
                    }
                }else{
                    if(pNew[0] != pOld[1]){
                        //cout << "DIVERSE3" << pOld[1] <<" ";
                        out = false;
                    }
                }          

            }else if(count == 3){

                if(pNew[1] != e && pNew[0] == e && pNew[2] == e){
                    piece temp = pNew[1];
                    pNew[1] = pNew[0];
                    pNew[0] = temp;

                    temp = pOld[1];
                    pOld[1] = pOld[0];
                    pOld[0] = temp;

                    pair<int,int> temp1 = pos[1];
                    pos[1] = pos[0];
                    pos[0] = temp1;
                }else if(pNew[2] != e && pNew[0] == e && pNew[1] == e){
                    piece temp = pNew[2];
                    pNew[2] = pNew[0];
                    pNew[0] = temp;

                    temp = pOld[2];
                    pOld[2] = pOld[0];
                    pOld[0] = temp;

                    pair<int,int> temp1 = pos[2];
                    pos[2] = pos[0];
                    pos[0] = temp1;
                }else if(!(pNew[0] != e && pNew[1] == e && pNew[2] == e)){
                    //cout << "NEsuna PossibILITA" << pNew[0] << " " << pOld[1] << " " << pOld[2] << " ";
                    out = false;   
                }

                if(pNew[0] == pOld[1]){
                    piece temp = pNew[1];
                    pNew[1] = pNew[2];
                    pNew[2] = temp;

                    temp = pOld[1];
                    pOld[1] = pOld[2];
                    pOld[2] = temp;

                    pair<int,int> temp1 = pos[1];
                    pos[1] = pos[2];
                    pos[2] = temp1;
                }

                if(pNew[0] == x){
                    if(pOld[1] != o){
                        //cout << "MANGIATO1";
                        out = false;
                    }
                    if(pOld[2] != pNew[0]){
                        //cout << "CAMBIATO1";
                        out = false;
                    }
                    if(pos[0].second == 0){
                        //cout << "!PROMOSSO";
                        out = false;
                    }
                    
                }else if(pNew[0] == o){
                    if(pOld[1] != x){
                        //cout << "MANGIATO2";
                        out = false;
                    }
                    if(pOld[2] != pNew[0]){
                        //cout << "CAMBIATO2";
                        out = false;
                    }
                    if(pos[0].second == 7){
                        //cout << "!PROMOSSO";
                        out = false;
                    }
                }else if(pNew[0] == X){
                    if(pOld[1] != o && pOld[1] != O){
                        //cout << "MANGIATO3" << "  " << pOld[1] << "  ";
                        out = false;
                    }
                    if(pos[0].second == 0){
                        if(pOld[2] != x && pOld[2] != X){
                            //cout << "CAMBIATO3";
                            out = false;
                        }
                    }else if(pOld[2] != pNew[0]){
                        //cout << "CAMBIATO4";
                        out = false;
                    }
                }else if(pNew[0] == O){
                    if(pOld[1] != x && pOld[1] != X){
                        //cout << "MANGIATO4" << "  " << pOld[1] << "  ";
                        out = false;
                    }
                    if(pos[0].second == 7){
                        if(pOld[2] != o && pOld[2] != O){
                            //cout << "CAMBIATO4";
                            out = false;
                        }
                    }else if(pOld[2] != pNew[0]){
                        //cout << "CAMBIATO4";
                        out = false;
                    }
                }

                int dirX = pos[1].first - pos[0].first, dirY = pos[1].second - pos[0].second;
                int dirX2 = pos[2].first - pos[1].first, dirY2 = pos[2].second - pos[1].second;

                if(dirX != dirX2 || dirY != dirY2){
                    //cout << "CAMBIODIR";
                    out = false;
                }
                
                if(dirX != 1 && dirX != -1){
                    //cout << "DIR X";
                    out = false;
                }
                
                if(pNew[0] == x){
                    if(dirY != 1){
                        //cout << "DIR 1";
                        out = false;
                    }
                }else if(pNew[0] == o){
                    if(dirY != -1){
                        //cout << "DIR 2";
                        out = false;
                    }
                }else{
                    if(dirY != 1 && dirY != -1){
                        //cout << "DIR 3";
                        out = false;
                    }
                }
            }else{
                //cout << "TROPPI";
                out = false;
            }

            delete [] pos;
            delete [] pOld;
            delete [] pNew;

            return out;
        }

    }else{
        throw player_exception{player_exception::index_out_of_bounds, "History con meno di due boards"};
    }

    return true;
}

void Player::pop(){
    pimpl->pop();
}

bool Player::wins(int player_nr) const{
    if(pimpl->head == nullptr){
        throw player_exception{player_exception::index_out_of_bounds, "History vuota"};
    }else if(player_nr < 1 || player_nr > 2){
        throw player_exception{player_exception::index_out_of_bounds, "Player number errato"};
    }

    //controllo per numero pezzi
    pair<int, int> count = pimpl->pezziRimasti(pimpl->head->board);

    if(player_nr == 1){
        if(count.second == 0){
            return true;
        }else{
            return false;
        }
    }else{
        if(count.first == 0){
            return true;
        }else{
            return false;
        }
    }
}

bool Player::wins() const{
    return wins(pimpl->player_nr);
}

bool Player::loses(int player_nr) const{
    if(pimpl->head == nullptr){
        throw player_exception{player_exception::index_out_of_bounds, "History vuota"};
    }else if(player_nr < 1 || player_nr > 2){
        throw player_exception{player_exception::index_out_of_bounds, "Player number errato"};
    }

    //controllo per numero pezzi
    pair<int, int> count = pimpl->pezziRimasti(pimpl->head->board);

    if(player_nr == 1){
        if(count.first == 0){
            return true;
        }else{
            return false;
        }
    }else{
        if(count.second == 0){
            return true;
        }else{
            return false;
        }
    }
}

bool Player::loses() const{
    return loses(pimpl->player_nr);
}

int Player::recurrence() const{
    Player::Impl::Cell* temp = pimpl->head;
    int count = 0;

    if(pimpl->head == nullptr){
        throw player_exception{player_exception::index_out_of_bounds, "History vuota"};
    }
    while(temp != nullptr){
        if(pimpl->compare_boards(temp->board)){
            count++;
        }
        temp = temp->next;
    }

    return count;
}

string Player::Impl::pString(piece p){
    string out = "";

    switch(p){
        case x: out = "x"; break;
        case X: out = "X"; break;
        case o: out = "o"; break;
        case O: out = "O"; break;
        case e: out = " "; break;
    }

    return out;
}
#include "player.hpp"

int main(){
    try{
    // std::cout << "p1" << std::endl;
    // Player p(1), p2(2);
    Player p(1);
    Player p2(2);
    // std::cout << "p2" << std::endl;
    std::string board_name =  "board_1.txt";
    int round = 1;
    
	p.init_board(board_name);
    p2.load_board(board_name);
    // p = p2;
    // p2.load_board("board_2.txt");
    std::cout << "test: " << p2(4, 0, 0) << std::endl;
    // std::cout << "test: " << p2(4, 0, 1) << std::endl;
    // std::cout << "test: " << p2(4, 0, 2) << std::endl;
    std::cout << std::boolalpha;
    while(round < 50){
        p.load_board(board_name);
        p.move();
        round++;
        std::cout << "round: " << round << " valida: " << p.valid_move() << std::endl;
        board_name =  "board_" + std::to_string(round) + ".txt";
        p.store_board(board_name,0);

        // std::cout << p.valid_move() << std::endl;

        p2.load_board(board_name);
        p2.move();
        round++;
        std::cout << "round: " << round << " valida: " << p2.valid_move() << std::endl;
        board_name =  "board_" + std::to_string(round) + ".txt";
        p2.store_board(board_name,0);
    }

    // p.load_board(board_name);

    // while(round < 35){
    //     round++;
    //     board_name =  "board_" + std::to_string(round) + ".txt";
    //     p.load_board(board_name);
    //     std::cout << "round: " << round << " valida: " << p.valid_move() << std::endl;
    // }
    }catch(player_exception p){
        std::cout << p.msg;
    }
    return 0;
}
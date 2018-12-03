#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>


int main(int argc, char *argv[]) {

    unsigned int counter = 0;
    const unsigned int count_up_to = 10000000;
    while (counter < count_up_to) {
        SDL_Log("counter value is: %d\n", counter);
        counter += 1;
    }

    return EXIT_SUCCESS;
}


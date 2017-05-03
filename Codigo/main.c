#include "readfile.c"
#include "reconizer.c"

int main(int argc, char *argv[])
{
    ps_decoder_t *ps;
    FILE *audio;
    char const *cmd;

    ps = init_reconizer();
    audio = read_file(argv[1]);

    cmd = reconize_it(ps, audio);
    printf("Recognized: %s\n", cmd);

    //do_something(cmd);

    return 0;
}

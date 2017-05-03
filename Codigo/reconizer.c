#include <stdio.h>
#include <stdlib.h>
#include <pocketsphinx.h>

#define MODELDIR ""

ps_decoder_t * init_reconizer()
{
    ps_decoder_t *ps;
    cmd_ln_t *config;

    config = cmd_ln_init(NULL, ps_args(), TRUE,
                 "-hmm", MODELDIR "/en-us/en-us",
                 "-lm", MODELDIR "/en-us/en-us.lm.bin",
                 "-dict", MODELDIR "/en-us/cmudict-en-us.dict",
                 NULL);
    if (config == NULL) {
        fprintf(stderr, "Failed to create config object, see log for details\n");
        exit(-1);
    }
    ps = ps_init(config);
    if (ps == NULL) {
        fprintf(stderr, "Failed to create recognizer, see log for details\n");
        exit(-1);
    }
    //cmd_ln_free_r(config);

    return ps;
}

char const* reconize_it(ps_decoder_t *ps, FILE *audio)
{
    char const *hyp, *uttid;
    int16 buf[512];
    int rv;
    int32 score;

    rv = ps_start_utt(ps);

    while (!feof(audio)) {
        size_t nsamp;
        nsamp = fread(buf, 2, 512, audio);
        rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
    }

    rv = ps_end_utt(ps);
    hyp = ps_get_hyp(ps, &score);
    printf("Recognized: %s\n", hyp);

    fclose(audio);
    ps_free(ps);

    return 0;
}

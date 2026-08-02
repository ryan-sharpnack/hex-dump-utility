/*
 * hexdump.c - Minimal hex dump utility (clone of `hexdump -C`)
 *
 * Reads a file using low-level POSIX I/O (open/read/close, no stdio
 * buffering) and prints each 16-byte row as:
 *   <8-digit hex offset>  <16 hex bytes, split into two groups of 8>  |ASCII|
 *
 * Usage: ./hexdump <file>
 */

#include <fcntl.h>   /* open()          */
#include <stdio.h>   /* printf/fprintf  */
#include <unistd.h>  /* read(), close() */

#define ROW_WIDTH 16 /* bytes displayed per output line */

/* Render one row: offset column, hex column, ASCII column. */
static void print_row(long offset, const unsigned char *buf, ssize_t len) {
    printf("%08lx  ", offset);

    for (int i = 0; i < ROW_WIDTH; i++) {
        if (i < len)
            printf("%02x ", buf[i]);
        else
            printf("   ");   /* keep columns aligned on the final short row */
        if (i == 7)
            printf(" ");     /* extra gap between the two 8-byte groups */
    }

    printf(" |");
    for (int i = 0; i < len; i++) {
        unsigned char c = buf[i];
        /* Printable ASCII range; everything else shown as '.' */
        putchar((c >= 0x20 && c < 0x7f) ? (int)c : '.');
    }
    printf("|\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        return 1;
    }

    unsigned char buf[ROW_WIDTH];
    ssize_t n;
    long offset = 0;

    while ((n = read(fd, buf, ROW_WIDTH)) > 0) {
        print_row(offset, buf, n);
        offset += n;
    }

    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    printf("%08lx\n", offset); /* trailing offset line, as in `hexdump -C` */
    close(fd);
    return 0;
}

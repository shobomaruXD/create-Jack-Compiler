#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_LEN 1024

// 行末の改行・空白を取り除く関数
// void trim_newline_and_spaces(char *line) {
//     int len = strlen(line);
//     while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' || isspace((unsigned char)line[len - 1]))) {
//         line[--len] = '\0';
//     }
// }

void compare_files_line_by_line(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");

    if (!fp1 || !fp2) {
        perror("ファイルを開けませんでした");
        return;
    }

    char line1[LINE_LEN], line2[LINE_LEN];
    int line_number = 1;
    int found_difference = 0;

    while (1) {
        char *res1 = fgets(line1, LINE_LEN, fp1);
        char *res2 = fgets(line2, LINE_LEN, fp2);

        if (!res1 && !res2) break;

        // 改行・空白削除
        // if (res1) trim_newline_and_spaces(line1);
        // if (res2) trim_newline_and_spaces(line2);

        if (!res1 || !res2 || strcmp(line1, line2) != 0) {
            found_difference = 1;
            printf("違いが見つかりました（%d行目）:\n", line_number);

            if (res1) printf("  file1: %s\n", line1);
            else      printf("  file1: <EOF>\n");

            if (res2) printf("  file2: %s\n", line2);
            else      printf("  file2: <EOF>\n");

            printf("\n");
        }

        line_number++;
    }

    if (!found_difference) {
        printf("ファイルは完全に一致しています。\n");
    }

    fclose(fp1);
    fclose(fp2);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "使い方: %s file1 file2\n", argv[0]);
        return 1;
    }

    compare_files_line_by_line(argv[1], argv[2]);
    return 0;
}

//
// Created by mehran on 2/27/18.
//

void refine()
{

    //FILE*f = fopen("white.list","w");

    FILE *f = fopen("dataset", "w");

    FILE *ids = fopen("ids", "w");

    for (int i = 0; i < lis.Len(); i++) {
        int j = lis[i];
        /*fprintf(f, "%s\n", listi[j].c_str());
        char str[80];
        sprintf(str, "copy papers_text\\%s\.txt white_papers_text\\%s\.txt", listi[j].c_str(), listi[j].c_str());
        system(str);*/
        char fname[80];
        sprintf(fname, "newUrl\\%s\.txt", listi[j].c_str());
        std::ifstream doc(fname);
        std::string str((std::istreambuf_iterator<char>(doc)),
                        std::istreambuf_iterator<char>());

        std::replace(str.begin(), str.end(), '\n', ' ');
        std::replace(str.begin(), str.end(), '\r', ' ');
        int count = 0, spaces = 0;

        std::replace_if(str.begin(), str.end(), [&count, &spaces](char x) {
            if (x < 0) {
                spaces++;
                return true;
            }
            if (isalpha(x)) {
                count++;
                return false;
            }
            switch (x) {
                case '(':
                    count++;
                    return false;
                case ')':
                    count++;
                    return false;
                case ';':
                    count++;
                    return false;
                case ',':
                    count++;
                    return false;
                case '\'':
                    count++;
                    return false;
                case '"':
                    count++;
                    return false;
                case '.':
                    count++;
                    return false;
                case '\\':
                    count++;
                    return false;
                case '/':
                    count++;
                    return false;
                case '[':
                    count++;
                    return false;
                case ']':
                    count++;
                    return false;
                case '?':
                    count++;
                    return false;
                case '!':
                    count++;
                    return false;
                case ':':
                    count++;
                    return false;
                case ' ':
                    spaces++;
                    count++;
                    return false;
                default:
                    spaces++;
                    return true;
            }
        }, ' ');

        if (count - spaces > 100) {
            fprintf(f, "%s\n", str.c_str());
            fprintf(ids, "%s\n", listi[j].c_str());
        } else {
            char str[80];
            sprintf(str, "copy papers_text\\%s\.txt black\\%s\.txt", listi[j].c_str(), listi[j].c_str());
            system(str);
        }
    }
    fclose(f);

}

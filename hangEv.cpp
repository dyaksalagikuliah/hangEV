/**
 * HangEV - Versi Lengkap Sesuai Dokumen Proyek
 *
 * Deskripsi:
 * Program ini adalah implementasi permainan Hangman dalam bahasa C yang dirancang
 * untuk sepenuhnya sesuai dengan spesifikasi proyek yang diberikan dalam file PDF.
 * Fitur utama meliputi antarmuka berbasis teks dengan panel, skor, alur permainan
 * berkelanjutan, dan seni ASCII kustom.
 *
 * Cara Kompilasi (menggunakan GCC):
 * gcc HangEv_Complete.c -o HangEv.exe
 *
 * Pastikan file "wordlist.txt" berada di direktori yang sama dengan file eksekusi.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>

// Struktur untuk menyimpan status permainan saat ini
typedef struct {
    char secretWord[50];
    char guessedWord[50];
    char wrongChars[7];
    int wrongGuesses;
    int score;
} GameState;

// --- PROTOTIPE FUNGSI ---

// Fungsi utilitas untuk kontrol konsol
void gotoxy(int x, int y);
void setConsoleColor(int color);

// Fungsi untuk menggambar elemen UI
void drawBox(int x1, int y1, int x2, int y2);
void drawHangman(int wrongGuesses);
void drawInitialGameUI();
void showGameOverScreen(int finalScore);
void showPauseScreenArt();

// Fungsi untuk menampilkan menu
int showMainMenu();
int showPauseMenu();

// Fungsi untuk memperbarui data di UI
void updateScore(int score);
void updateWrongChars(const char* chars);
void updateWord(const char* word);
void updateSelectedChar(char c);
void clearSelectedChar();
void showRoundWinMessage(const char* secretWord);

// Fungsi utama logika permainan
void playGame(char** wordList, int wordCount);
char** loadWords(int* wordCount);
char* getRandomWord(char** wordList, int wordCount);
void freeWords(char** words, int wordCount);


// --- FUNGSI MAIN ---
int main() {
    srand(time(NULL)); // Inisialisasi random seed
    int wordCount = 0;
    char** words = loadWords(&wordCount);

    if (words == NULL) {
        printf("Gagal memuat wordlist.txt. Pastikan file ada di direktori yang sama.\n");
        printf("Tekan tombol apa saja untuk keluar...");
        getch();
        return 1;
    }

    int menuChoice;
    do {
        system("cls");
        menuChoice = showMainMenu();
        if (menuChoice == 1) {
            playGame(words, wordCount);
        }
    } while (menuChoice != 2);

    freeWords(words, wordCount);
    system("cls");
    printf("\nTerima kasih telah bermain!\n");
    return 0;
}

// --- IMPLEMENTASI FUNGSI ---

/**
 * Memindahkan kursor ke koordinat (x, y) di konsol.
 */
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/**
 * Mengatur warna teks dan background konsol.
 */
void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/**
 * Menggambar kotak dengan garis ASCII.
 */
void drawBox(int x1, int y1, int x2, int y2) {
    int i;
    gotoxy(x1, y1); printf("%c", 201); // Sudut kiri atas
    for (i = x1 + 1; i < x2; i++) printf("%c", 205); // Garis atas
    gotoxy(x2, y1); printf("%c", 187); // Sudut kanan atas

    for (i = y1 + 1; i < y2; i++) {
        gotoxy(x1, i); printf("%c", 186); // Garis kiri
        gotoxy(x2, i); printf("%c", 186); // Garis kanan
    }

    gotoxy(x1, y2); printf("%c", 200); // Sudut kiri bawah
    for (i = x1 + 1; i < x2; i++) printf("%c", 205); // Garis bawah
    gotoxy(x2, y2); printf("%c", 188); // Sudut kanan bawah
}

/**
 * Menggambar hangman sesuai dengan jumlah tebakan salah.
 */
void drawHangman(int wrongGuesses) {
    // Bersihkan area hangman sebelum menggambar
    for(int i = 3; i < 15; i++){
        gotoxy(5, i);
        printf("                  ");
    }

    // Tiang gantungan (selalu ada)
    gotoxy(5, 18); printf("=================================");
    for(int i = 3; i < 18; i++){
    }
    gotoxy(5, 3); printf("||==============T================");
    
    
    gotoxy(21,4); printf("|");
    gotoxy(21,5); printf("|");
    gotoxy(21,6); printf("|");
    // gotoxy(21,7); printf("|");


    switch (wrongGuesses) {
        case 6: // Kaki kanan
            gotoxy(22, 13); printf(" |");
            gotoxy(22, 14); printf(" |");
            gotoxy(22, 15); printf("o|");
            Sleep(1000);
        case 5: // Kaki kiri
            gotoxy(19,13);printf("| |");
            gotoxy(19,14);printf("| |");
            gotoxy(19,15);printf("|o|");
            // Sleep(1000);
        case 4: // Tangan kanan
            gotoxy(22, 12); printf("w|");
            gotoxy(22, 10); printf("|");
            gotoxy(22, 11); printf("|");
            gotoxy(22, 9); printf("_");
        case 3: // Tangan kiri
            gotoxy(17, 12); printf("|w");
            gotoxy(17, 10); printf("| |");
            gotoxy(17, 11); printf("| |");
            gotoxy(17, 9); printf(" _");
        case 2: // Badan
            gotoxy(19, 10); printf("|   |");
            gotoxy(19, 11); printf("|   |");
            gotoxy(19, 12); printf("|___|");

        case 1: // Kepala
            gotoxy(19, 7); printf("~+-+~");
            gotoxy(19, 8); printf("|E V|");
            gotoxy(19, 9); printf("|_^_|");
            break;            break;
    }
}

/**
 * Menampilkan menu utama permainan.
 */
int showMainMenu() {
    int selection = 1;
    char key;

    while (1) {
        system("cls");
        setConsoleColor(14); // Putih
        gotoxy(15, 2);
        printf(" __    __       ___      .__   __.   _______  ___________    ____");
        gotoxy(15,3);
        printf("|  |  |  |     /   \\     |  \\ |  |  /  _____||   ____\\   \\  /   /");
        gotoxy(15,4);
        printf("|  |__|  |    /  ^  \\    |   \\|  | |  |  __  |  |__   \\   \\/   /");
        gotoxy(15,5);
        printf("|   __   |   /  /_\\  \\   |  . `  | |  | |_ | |   __|   \\      /");
        gotoxy(15,6);
        printf("|  |  |  |  /  _____  \\  |  |\\   | |  |__| | |  |____   \\    /");
        gotoxy(15,7);
        printf("|__|  |__| /__/     \\__\\ |__| \\__|  \\______| |_______|   \\__/");

        gotoxy(15, 9);
        printf("=============================================================");

        gotoxy(32, 11);
        setConsoleColor(selection == 1 ? 10 : 15); // Hijau terang jika dipilih
        printf("%s 1. Play hangEV", (selection == 1) ? ">>" : "  ");

        gotoxy(32, 12);
        setConsoleColor(selection == 2 ? 10 : 15); // Hijau terang jika dipilih
        printf("%s 2. Exit Game", (selection == 2) ? ">>" : "  ");
        setConsoleColor(15);

        key = getch();
        if (key == 72 || tolower(key) == 'w') { // Panah atas atau W
            if (selection > 1) selection--;
        } else if (key == 80 || tolower(key) == 's') { // Panah bawah atau S
            if (selection < 2) selection++;
        } else if (key == 13) { // Enter
            return selection;
        }
    }
}

/**
 * Logika utama permainan.
 */
void playGame(char** wordList, int wordCount) {
    GameState currentState;
    currentState.score = 0;

    // Loop utama untuk setiap ronde permainan
    while (1) {
        // Inisialisasi state untuk ronde baru
        char* secret = getRandomWord(wordList, wordCount);
        strcpy(currentState.secretWord, secret);
        strcpy(currentState.guessedWord, secret);
        for (int i = 0; i < strlen(currentState.guessedWord); i++) {
            currentState.guessedWord[i] = '_';
        }
        currentState.wrongGuesses = 0;
        currentState.wrongChars[0] = '\0';
        int wordGuessed = 0;

        // Gambar UI awal
        drawInitialGameUI();
        updateScore(currentState.score);
        updateWrongChars(currentState.wrongChars);
        updateWord(currentState.guessedWord);

        // Loop untuk satu kata
        while (currentState.wrongGuesses < 6 && !wordGuessed) {
            drawHangman(currentState.wrongGuesses);

            gotoxy(48, 15); printf(" "); // Posisi input
            char guess = getch();

            if (guess == 27) { // Tombol ESC
                int pauseChoice = showPauseMenu();
                if (pauseChoice == 2) { // Pilih Exit
                    showGameOverScreen(currentState.score);
                    return; // Keluar dari fungsi playGame
                }
                // Jika pilih Continue, gambar ulang UI
                drawInitialGameUI();
                updateScore(currentState.score);
                updateWrongChars(currentState.wrongChars);
                updateWord(currentState.guessedWord);
                continue;
            }

            if (!isalpha(guess)) continue; // Hanya proses input alfabet
            guess = tolower(guess);

            updateSelectedChar(toupper(guess)); // Tampilkan huruf yg dipilih
            
            // Cek apakah huruf sudah pernah ditebak
            if (strchr(currentState.guessedWord, guess) || strchr(currentState.wrongChars, guess)) {
                // Bisa ditambahkan pesan "Sudah ditebak" jika diinginkan
                Sleep(500);
                clearSelectedChar();
                continue;
            }

            int letterFound = 0;
            for (int i = 0; i < strlen(currentState.secretWord); i++) {
                if (currentState.secretWord[i] == guess) {
                    currentState.guessedWord[i] = guess;
                    letterFound = 1;
                }
            }

            if (!letterFound) {
                int len = strlen(currentState.wrongChars);
                if (len < 6) {
                    currentState.wrongChars[len] = toupper(guess);
                    currentState.wrongChars[len + 1] = '\0';
                    currentState.wrongGuesses++;
                    updateWrongChars(currentState.wrongChars);
                }
            }

            updateWord(currentState.guessedWord);
            
            // Cek apakah kata sudah tertebak semua
            if (strchr(currentState.guessedWord, '_') == NULL) {
                wordGuessed = 1;
            }
            
            Sleep(200);
            clearSelectedChar();
        } // Akhir loop satu kata

        // Setelah loop satu kata selesai
        if (wordGuessed) {
            currentState.score += 10; // Tambah skor
            showRoundWinMessage(currentState.secretWord);
            // Permainan akan otomatis lanjut ke ronde berikutnya (loop while(1))
        } else {
            // Jika kalah
            drawHangman(currentState.wrongGuesses);
            showGameOverScreen(currentState.score);
            return; // Keluar dari fungsi playGame
        }
    }
}

/**
 * Menggambar seluruh antarmuka permainan di awal.
 */
void drawInitialGameUI() {
    system("cls");
    setConsoleColor(15);

    // Panel Skor
    drawBox(40, 2, 65, 5);
    gotoxy(42, 3); printf("Score");

    // Panel Karakter Salah
    drawBox(40, 6, 65, 9);
    gotoxy(42, 7); printf("Wrong Character");

    // Panel Pilih Karakter
    drawBox(40, 10, 65, 13);
    gotoxy(42, 11); printf("Select Character");

    // Panel Kata
    drawBox(40, 14, 65, 17);
    gotoxy(42, 15); printf("Word");
}


/**
 * Memuat kata dari file wordlist.txt.
 */
char** loadWords(int* wordCount) {
    FILE* fp = fopen("wordlist.txt", "r");
    if (fp == NULL) {
        return NULL;
    }

    char buffer[100];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        count++;
    }
    *wordCount = count;
    rewind(fp);

    char** wordlist = (char**)malloc(count * sizeof(char*));
    if (wordlist == NULL) {
        fclose(fp);
        return NULL;
    }

    int i = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Hapus newline
        for(int j = 0; buffer[j]; j++){
            buffer[j] = tolower(buffer[j]);
        }
        wordlist[i] = (char*)malloc(strlen(buffer) + 1);
        strcpy(wordlist[i], buffer);
        i++;
    }

    fclose(fp);
    return wordlist;
}

/**
 * Mengambil satu kata acak dari daftar kata.
 */
char* getRandomWord(char** wordList, int wordCount) {
    return wordList[rand() % wordCount];
}

/**
 * Membebaskan memori yang dialokasikan untuk daftar kata.
 */
void freeWords(char** words, int wordCount) {
    if (words == NULL) return;
    for (int i = 0; i < wordCount; i++) {
        free(words[i]);
    }
    free(words);
}

/**
 * Menampilkan seni ASCII untuk layar pause.
 */
void showPauseScreenArt() {
    gotoxy(15, 5); printf(" ____   __   _  _  ____  ____ ");
    gotoxy(15, 6); printf("(  _ \\ / _\\ ( \\/ )( ___)(  _ \\");
    gotoxy(15, 7); printf(" ) __//    \\/ \\/ \\ )__)  )   /");
    gotoxy(15, 8); printf("(__)  \\_/\\_/\\_)(_/(____)(__\\_)");
}


/**
 * Menampilkan menu pause.
 */
int showPauseMenu() {
    int selection = 1;
    char key;

    while (1) {
        system("cls");
        setConsoleColor(14); // Kuning
        showPauseScreenArt();
        setConsoleColor(15); // Putih
        gotoxy(20, 12);
        
        setConsoleColor(selection == 1 ? 10 : 15);
        printf("%s 1. Continue the game", (selection == 1) ? ">>" : "  ");

        gotoxy(20, 13);
        setConsoleColor(selection == 2 ? 10 : 15);
        printf("%s 2. Exit the game", (selection == 2) ? ">>" : "  ");
        setConsoleColor(15);

        key = getch();
        if (key == 72 || tolower(key) == 'w') { // Panah atas atau W
            if (selection > 1) selection--;
        } else if (key == 80 || tolower(key) == 's') { // Panah bawah atau S
            if (selection < 2) selection++;
        } else if (key == 13) { // Enter
            return selection;
        }
    }
}

/**
 * Menampilkan layar Game Over.
 */
void showGameOverScreen(int finalScore) {
    system("cls");
    setConsoleColor(12); // Merah
    gotoxy(10, 5); printf("  ____    _    __  __  ____    ____   _   _  ____  ____  ");
    gotoxy(10, 6); printf(" / ___|  / \\  |  \\/  |/ ___|  / ___| | | | |/ ___||  _ \\ ");
    gotoxy(10, 7); printf("| |  _  / _ \\ | |\\/| | |  _  | |   | | | | | |  _ | |_) |");
    gotoxy(10, 8); printf("| |_| |/ ___ \\| |  | | |_| | | |___| | |_| | |_| ||  _ < ");
    gotoxy(10, 9); printf(" \\____/_/   \\_\\_|  |_|\\____|  \\____|  \\___/ \\____||_| \\_\\");
    
    setConsoleColor(15);
    gotoxy(30, 12);
    printf("Final Score: %d", finalScore);
    gotoxy(28, 14);
    printf("Press any key to return to main menu...");
    getch();
}

// --- FUNGSI-FUNGSI UPDATE UI ---

void updateScore(int score) {
    gotoxy(48, 4);
    printf("      %d", score);
}

void updateWrongChars(const char* chars) {
    gotoxy(45, 8);
    printf("                "); // Clear previous
    gotoxy(45, 8);
    for(int i = 0; i < strlen(chars); i++){
        printf("%c ", chars[i]);
    }
}

void updateWord(const char* word) {
    gotoxy(45, 16);
    printf("                "); // Clear previous
    gotoxy(45, 16);
    for(int i = 0; i < strlen(word); i++){
        printf("%c ", toupper(word[i]));
    }
}

void updateSelectedChar(char c) {
    gotoxy(48, 12);
    printf("%c", c);
}

void clearSelectedChar(){
    gotoxy(48, 12);
    printf(" ");
}

void showRoundWinMessage(const char* secretWord) {
    gotoxy(5, 16);
    setConsoleColor(10);
    printf("Selamat! Anda menebak kata: %s", secretWord);
    setConsoleColor(15);
    Sleep(2000); // Tampilkan pesan selama 2 detik
}


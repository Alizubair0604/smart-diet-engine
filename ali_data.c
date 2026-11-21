#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "food_structures.h"

#define MAX_LINE   16384
#define MAX_FIELDS 40

// break a csv row into separate columns, keeping quoted commas and empty cells
// strtok didn't work well here, so we parse 'line' directly in place
static int split_csv_line(char *line, char *fields[], int max_fields) {
    int count = 0;
    char *p = line;

    while (count < max_fields) {
        char *out;
        if (*p == '"') {                       // if field starts with quotes
            p++;
            fields[count++] = p;
            out = p;
            while (*p) {
                if (*p == '"' && *(p + 1) == '"') { *out++ = '"'; p += 2; }   // double quote inside quotes
                else if (*p == '"') { p++; break; }                           // end of quoted text
                else *out++ = *p++;
            }
            while (*p && *p != ',') p++;       // move pointer to next comma
            *out = '\0';
        } else {                               // normal unquoted value (can be blank)
            fields[count++] = p;
            while (*p && *p != ',') p++;
            if (*p == ',') { *p = '\0'; p++; fields[count - 1] = fields[count - 1]; }
            else { *p = '\0'; return count; }
            continue;
        }
        if (*p == ',') p++;
        else return count;
    }
    return count;
}

// look up column index by header name, return -1 if not found
static int find_column(char *fields[], int n, const char *name) {
    for (int i = 0; i < n; i++)
        if (strcmp(fields[i], name) == 0) return i;
    return -1;
}

// read food items from csv file (-1 if opening or header check fails)
// scale macro values between 0.0 and 1.0 before storing
int load_nutritional_data(const char *filename, FoodItem dataset[], int max_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open dataset file %s\n", filename);
        return -1;
    }

    static char line[MAX_LINE];
    char *fields[MAX_FIELDS];

  // read first row to find where each required column sits
    if (!fgets(line, sizeof(line), file)) { fclose(file); return 0; }
    line[strcspn(line, "\r\n")] = '\0';
    int nh = split_csv_line(line, fields, MAX_FIELDS);

    int c_name = find_column(fields, nh, "food_name");
    int c_carb = find_column(fields, nh, "carbs_g");
    int c_fat  = find_column(fields, nh, "fat_g");
    int c_prot = find_column(fields, nh, "protein_g");
    if (c_name < 0 || c_carb < 0 || c_fat < 0 || c_prot < 0) {
        printf("Error: CSV header is missing a required column.\n");
        fclose(file);
        return -1;
    }

    int need = c_name;
    if (c_carb > need) need = c_carb;
    if (c_fat  > need) need = c_fat;
    if (c_prot > need) need = c_prot;

    int count = 0;
    while (count < max_size && fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        int n = split_csv_line(line, fields, MAX_FIELDS);
        if (n <= need) continue;

        // ignore rows missing food name or any macro value
        if (fields[c_name][0] == '\0' ||
            fields[c_carb][0] == '\0' || fields[c_fat][0] == '\0' ||
            fields[c_prot][0] == '\0') continue;

        double carb = atof(fields[c_carb]);
        double fat  = atof(fields[c_fat]);
        double prot = atof(fields[c_prot]);

       // dataset calorie column has mixed units (kcal/kJ), so calculate from macros
       // using 4 kcal/g for protein and carbs, 9 kcal/g for fat 
        double cal = 4.0 * prot + 4.0 * carb + 9.0 * fat;

       // filter out unrealistic values per 100g (macros > 100g or cal > 900)
        if (cal > CALORIES_SCALE || carb < 0 || carb > CARBS_SCALE ||
            fat < 0 || fat > FATS_SCALE || prot < 0 || prot > PROTEIN_SCALE) continue;

        strncpy(dataset[count].name, fields[c_name], sizeof(dataset[count].name) - 1);
        dataset[count].name[sizeof(dataset[count].name) - 1] = '\0';
        dataset[count].protein  = prot / PROTEIN_SCALE;
        dataset[count].carbs    = carb / CARBS_SCALE;
        dataset[count].fats     = fat  / FATS_SCALE;
        dataset[count].calories = cal  / CALORIES_SCALE;
        count++;
    }

    fclose(file);
    return count;
}

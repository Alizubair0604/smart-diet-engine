#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "food_structures.h"

#define MAX_LINE   16384
#define MAX_FIELDS 40

// Splits one CSV line into fields. Handles "quoted, fields" and EMPTY fields
// correctly (strtok cannot do either). Modifies 'line' in place.
static int split_csv_line(char *line, char *fields[], int max_fields) {
    int count = 0;
    char *p = line;

    while (count < max_fields) {
        char *out;
        if (*p == '"') {                       // quoted field
            p++;
            fields[count++] = p;
            out = p;
            while (*p) {
                if (*p == '"' && *(p + 1) == '"') { *out++ = '"'; p += 2; }   // escaped quote
                else if (*p == '"') { p++; break; }                           // closing quote
                else *out++ = *p++;
            }
            while (*p && *p != ',') p++;       // skip to the comma
            *out = '\0';
        } else {                               // plain field (may be empty)
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

// Finds the column number of a header name (e.g. "protein_g"). Returns -1 if missing.
static int find_column(char *fields[], int n, const char *name) {
    for (int i = 0; i < n; i++)
        if (strcmp(fields[i], name) == 0) return i;
    return -1;
}

// Loads the nutrition CSV. Returns the number of foods loaded, or -1 on error.
// Nutrient values are normalized to 0.0 - 1.0 (see food_structures.h).
int load_nutritional_data(const char *filename, FoodItem dataset[], int max_size) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open dataset file %s\n", filename);
        return -1;
    }

    static char line[MAX_LINE];
    char *fields[MAX_FIELDS];

    // Read the header and locate the columns we need by NAME
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

        // Skip rows with a missing nutrient value or an empty name
        if (fields[c_name][0] == '\0' ||
            fields[c_carb][0] == '\0' || fields[c_fat][0] == '\0' ||
            fields[c_prot][0] == '\0') continue;

        double carb = atof(fields[c_carb]);
        double fat  = atof(fields[c_fat]);
        double prot = atof(fields[c_prot]);

        // The CSV "calories" column mixes kcal and kJ (e.g. cooked white rice shows 406),
        // so calories are computed from the macros instead (4/4/9 kcal per gram).
        double cal = 4.0 * prot + 4.0 * carb + 9.0 * fat;

        // Skip impossible values (per 100 g: max 100 g of any macro, max 900 kcal)
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
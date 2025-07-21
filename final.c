#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MEDICINES 10  // Maximum medicines per prescription
#define FILE_NAME "prescriptions.txt"

// Prescription structure
typedef struct Prescription {
    char name[50];
    char creation_date[15];
    int num_medicines;
    char medicines[MAX_MEDICINES][50];
    char dosages[MAX_MEDICINES][20];
    char schedules[MAX_MEDICINES][20];
    char expiry_dates[MAX_MEDICINES][15];
    struct Prescription *next;
} Prescription;

Prescription *head = NULL;

// Helper to trim newline
void trimNewline(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

// Get current date
void getCurrentDate(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);//struct is default structure day,time,date etc  local time is set to t
    strftime(buffer, 15, "%Y-%m-%d", tm_info);//strftime is a standard C library function used to format date and time into a string
}

// Save prescriptions to file
void saveToFile() {
    FILE *fp = fopen(FILE_NAME, "w");
    if (!fp) {
        printf("Failed to open file for writing.\n");
        return;
    }

    Prescription *temp = head;
    while (temp) {
        fprintf(fp, "%s\n%s\n%d\n", temp->name, temp->creation_date, temp->num_medicines);
        for (int i = 0; i < temp->num_medicines; i++) {
            fprintf(fp, "%s\n%s\n%s\n%s\n",
                    temp->medicines[i],
                    temp->dosages[i],
                    temp->schedules[i],
                    temp->expiry_dates[i]);
        }
        fprintf(fp, "---\n");  // Separator between prescriptions
        temp = temp->next;
    }

    fclose(fp);
}

// Load prescriptions from file
void loadFromFile() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return;

    char line[100];
    while (fgets(line, sizeof(line), fp)) {
        Prescription *newPrescription = (Prescription *)malloc(sizeof(Prescription));
        if (!newPrescription) {
            printf("Memory allocation failed while reading file.\n");
            fclose(fp);
            return;
        }

        trimNewline(line);
        strcpy(newPrescription->name, line);

        fgets(newPrescription->creation_date, sizeof(newPrescription->creation_date), fp);
        trimNewline(newPrescription->creation_date);

        fgets(line, sizeof(line), fp);
        newPrescription->num_medicines = atoi(line);

        for (int i = 0; i < newPrescription->num_medicines; i++) {
            fgets(newPrescription->medicines[i], sizeof(newPrescription->medicines[i]), fp);
            trimNewline(newPrescription->medicines[i]);

            fgets(newPrescription->dosages[i], sizeof(newPrescription->dosages[i]), fp);
            trimNewline(newPrescription->dosages[i]);

            fgets(newPrescription->schedules[i], sizeof(newPrescription->schedules[i]), fp);
            trimNewline(newPrescription->schedules[i]);

            fgets(newPrescription->expiry_dates[i], sizeof(newPrescription->expiry_dates[i]), fp);
            trimNewline(newPrescription->expiry_dates[i]);
        }

        fgets(line, sizeof(line), fp);  // Read separator line "---"

        newPrescription->next = head;
        head = newPrescription;
    }

    fclose(fp);
}

// Add new prescription
void addPrescription() {
    Prescription *newPrescription = (Prescription *)malloc(sizeof(Prescription));
    if (!newPrescription) {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter Patient Name: ");
    fgets(newPrescription->name, sizeof(newPrescription->name), stdin);
    trimNewline(newPrescription->name);

    getCurrentDate(newPrescription->creation_date);
    printf("Prescription Date: %s\n", newPrescription->creation_date);

    printf("Enter number of medicines (max %d): ", MAX_MEDICINES);
    scanf("%d", &newPrescription->num_medicines);
    getchar();

    if (newPrescription->num_medicines > MAX_MEDICINES) {
        printf("You can add up to %d medicines only.\n", MAX_MEDICINES);
        free(newPrescription);
        return;
    }

    for (int i = 0; i < newPrescription->num_medicines; i++) {
        printf("\nMedicine %d:\n", i + 1);

        printf("Enter Medicine Name: ");
        fgets(newPrescription->medicines[i], sizeof(newPrescription->medicines[i]), stdin);
        trimNewline(newPrescription->medicines[i]);

        printf("Enter Dosage: ");
        fgets(newPrescription->dosages[i], sizeof(newPrescription->dosages[i]), stdin);
        trimNewline(newPrescription->dosages[i]);

        printf("Enter Schedule: ");
        fgets(newPrescription->schedules[i], sizeof(newPrescription->schedules[i]), stdin);
        trimNewline(newPrescription->schedules[i]);

        printf("Enter Expiry Date (YYYY-MM-DD): ");
        fgets(newPrescription->expiry_dates[i], sizeof(newPrescription->expiry_dates[i]), stdin);
        trimNewline(newPrescription->expiry_dates[i]);
    }

    newPrescription->next = head;
    head = newPrescription;

    saveToFile();  // Save after adding
    printf("Prescription added successfully!\n");
}

// View all prescriptions
void viewPrescriptions() {
    if (!head) {
        printf("No prescriptions stored.\n");
        return;
    }

    Prescription *temp = head;
    printf("\nStored Prescriptions:\n");
    while (temp) {
        printf("\nPatient: %s | Prescription Date: %s\n", temp->name, temp->creation_date);
        for (int i = 0; i < temp->num_medicines; i++) {
            printf("  Medicine %d: %s | Dosage: %s | Schedule: %s | Expiry: %s\n",
                   i + 1, temp->medicines[i], temp->dosages[i], temp->schedules[i], temp->expiry_dates[i]);
        }
        temp = temp->next;
    }
}

// Check if expired
int isExpired(const char *date) {
    int y, m, d;
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3)
        return 0;

    time_t t = time(NULL);// store time to NULL
    struct tm *today = localtime(&t);// struct is default structure day,time,date etc  local time is set to t
    int currY = today->tm_year + 1900;// as year starts from 1900 as c was invented in 1900, and memory efficiency
    int currM = today->tm_mon + 1;// jan 0 month
    int currD = today->tm_mday;

    return (y < currY || (y == currY && m < currM) || (y == currY && m == currM && d < currD));
}

// Check expired medicines
void checkExpiry() {
    if (!head) {
        printf("No prescriptions stored.\n");
        return;
    }

    Prescription *temp = head;
    int found = 0;

    printf("\nExpired Medicines:\n");
    while (temp) {
        for (int i = 0; i < temp->num_medicines; i++) {
            if (isExpired(temp->expiry_dates[i])) {
                printf("\nPatient: %s | Prescription Date: %s\n", temp->name, temp->creation_date);
                printf("  Medicine %d: %s | Dosage: %s | Schedule: %s | Expiry: %s (EXPIRED)\n",
                       i + 1, temp->medicines[i], temp->dosages[i], temp->schedules[i], temp->expiry_dates[i]);
                found = 1;
            }
        }
        temp = temp->next;
    }

    if (!found)
        printf("No expired medicines.\n");
}

// Search prescription
void searchPrescription() {
    if (!head) {
        printf("No prescriptions stored.\n");
        return;
    }

    char searchName[50];
    printf("Enter Patient Name to search: ");
    fgets(searchName, sizeof(searchName), stdin);
    trimNewline(searchName);

    Prescription *temp = head;
    int found = 0;

    while (temp) {
        if (strcmp(searchName, temp->name) == 0) {
            printf("\nPatient: %s | Prescription Date: %s\n", temp->name, temp->creation_date);
            for (int i = 0; i < temp->num_medicines; i++) {
                printf("  Medicine %d: %s | Dosage: %s | Schedule: %s | Expiry: %s\n",
                       i + 1, temp->medicines[i], temp->dosages[i], temp->schedules[i], temp->expiry_dates[i]);
            }
            found = 1;
            break;
        }
        temp = temp->next;
    }

    if (!found)
        printf("No prescription found for %s.\n", searchName);
}

// Delete a prescription
void deletePrescription() {
    if (!head) {
        printf("No prescriptions stored.\n");
        return;
    }

    char delName[50];
    printf("Enter Patient Name to delete: ");
    fgets(delName, sizeof(delName), stdin);
    trimNewline(delName);

    Prescription *temp = head, *prev = NULL;

    while (temp) {
        if (strcmp(delName, temp->name) == 0) {
            if (!prev)
                head = temp->next;
            else
                prev->next = temp->next;

            free(temp);
            saveToFile();  // Save after deletion
            printf("Prescription deleted successfully!\n");
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    printf("No prescription found for %s.\n", delName);
}

// Free memory
void freeMemory() {
    Prescription *temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Main menu
int main() {
    loadFromFile();  // Load at startup

    int choice;
    while (1) {
        printf("\nSmart Prescription Manager\n");
        printf("1. Add Prescription\n");
        printf("2. View Prescriptions\n");
        printf("3. Search Prescription\n");
        printf("4. Check Expiry\n");
        printf("5. Delete Prescription\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();  // Consume newline

        switch (choice) {
            case 1: addPrescription(); break;
            case 2: viewPrescriptions(); break;
            case 3: searchPrescription(); break;
            case 4: checkExpiry(); break;
            case 5: deletePrescription(); break;
            case 6: freeMemory(); exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

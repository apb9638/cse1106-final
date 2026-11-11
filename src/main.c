#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef USE_PIGPIO
#include <pigpio.h>
#endif

#define LED_PIN 17
#define LOG_FILE "data/occupancy_log.csv"

int occupancy = 0;
int max_capacity = 5;
int led_is_on = 0;

void set_led(int turn_on)
{
    if (turn_on == led_is_on)
    {
        return;
    }

    led_is_on = turn_on;

#ifdef USE_PIGPIO
    gpioWrite(LED_PIN, turn_on ? 1 : 0);
#else
    printf("[LED SIMULATION] %s\n", turn_on ? "ON" : "OFF");
#endif
}

void update_led(void)
{
    set_led(occupancy >= max_capacity);
}

void show_status(void)
{
    printf("\nCurrent occupancy: %d/%d\n", occupancy, max_capacity);

    if (occupancy >= max_capacity)
    {
        printf("Room status: ROOM FULL\n");
    }
    else
    {
        printf("Room status: SPACE AVAILABLE\n");
    }

    printf("Full indicator LED: %s\n\n", led_is_on ? "ON" : "OFF");
}

void log_event(const char *event)
{
    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL)
    {
        printf("Error: Could not open the log file.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    char timestamp[30];

    if (local_time != NULL)
    {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);
    }
    else
    {
        snprintf(timestamp, sizeof(timestamp), "unknown");
    }

    fprintf(file, "%s,%s,%d,%d\n",
            timestamp, event, occupancy, max_capacity);
    fclose(file);
}

void person_enters(void)
{
    if (occupancy >= max_capacity)
    {
        printf("Cannot enter: the room is already full.\n");
        return;
    }

    occupancy++;
    log_event("ENTER");
    update_led();

    printf("One person entered successfully.\n");
    show_status();
}

void person_exits(void)
{
    if (occupancy <= 0)
    {
        printf("Cannot exit: the room is already empty.\n");
        return;
    }

    occupancy--;
    log_event("EXIT");
    update_led();

    printf("One person exited successfully.\n");
    show_status();
}

void change_capacity(void)
{
    int new_capacity;

    printf("Enter the new maximum capacity: ");

    if (scanf("%d", &new_capacity) != 1)
    {
        printf("Invalid input. Please enter a whole number.\n");
        while (getchar() != '\n' && !feof(stdin))
        {
        }
        return;
    }

    if (new_capacity <= 0)
    {
        printf("Capacity must be greater than zero. No change was made.\n");
        return;
    }

    if (new_capacity < occupancy)
    {
        printf("Capacity cannot be lower than the current occupancy (%d). No change was made.\n",
               occupancy);
        return;
    }

    max_capacity = new_capacity;
    log_event("CAPACITY_CHANGE");
    update_led();

    printf("Maximum capacity changed to %d.\n", max_capacity);
    show_status();
}

int main(void)
{
#ifdef USE_PIGPIO
    if (gpioInitialise() < 0)
    {
        printf("Error: pigpio could not initialize. Check Raspberry Pi setup.\n");
        return 1;
    }

    gpioSetMode(LED_PIN, PI_OUTPUT);
    gpioWrite(LED_PIN, 0);
#endif

    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL)
    {
        printf("Error: Could not create/open %s. Make sure the data folder exists.\n",
               LOG_FILE);
#ifdef USE_PIGPIO
        gpioTerminate();
#endif
        return 1;
    }

    /* Add the CSV heading only when the file is empty. */
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
    {
        fprintf(file, "Timestamp,Event,Occupancy,MaximumCapacity\n");
    }
    fclose(file);

    int choice;

    printf("====================================\n");
    printf("   SMART STUDY ROOM OCCUPANCY TRACKER\n");
    printf("====================================\n");

    do
    {
        printf("\n1. Record a person entering\n");
        printf("2. Record a person exiting\n");
        printf("3. Display room status\n");
        printf("4. Change maximum capacity\n");
        printf("5. Exit program\n");
        printf("Choose an option: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Please enter a number from 1 to 5.\n");
            while (getchar() != '\n' && !feof(stdin))
            {
            }
            continue;
        }

        switch (choice)
        {
        case 1:
            person_enters();
            break;
        case 2:
            person_exits();
            break;
        case 3:
            show_status();
            break;
        case 4:
            change_capacity();
            break;
        case 5:
            printf("Exiting program. Goodbye!\n");
            break;
        default:
            printf("Invalid option. Please choose 1 through 5.\n");
        }
    } while (choice != 5);

    set_led(0);

#ifdef USE_PIGPIO
    gpioTerminate();
#endif

    return 0;
}

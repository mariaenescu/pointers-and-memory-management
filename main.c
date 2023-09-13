// Enescu Maria
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"

extern void get_operations(void **operations);

/**
	Functie folosita pentru a initializa structura senzorului de tip tire
	cu valorile citite din fisierul binar.
**/
void create_tire_sensor(sensor *new_sensor, float pressure, float temperature,
						int wear_level, int performance_score,
						int nr_operations, int *operations_idxs)
{
	tire_sensor *tire = malloc(sizeof(tire_sensor));
	tire->pressure = pressure;
	tire->temperature = temperature;
	tire->wear_level = wear_level;
	tire->performace_score = performance_score;
	new_sensor->sensor_type = 0;
	new_sensor->sensor_data = (tire_sensor *)tire;
	new_sensor->nr_operations = nr_operations;
	new_sensor->operations_idxs = operations_idxs;
}

/**
	Functie folosita pentru a initializa structura senzorului de tip pmu
	cu valorile citite din fisierul binar.
**/
void create_pmu(sensor *new_sensor, float voltage, float current,
				float power_consumption, int energy_regen,
				int energy_storage, int nr_operations, int *operations_idxs)
{
	power_management_unit *pmu = malloc(sizeof(power_management_unit));
	pmu->voltage = voltage;
	pmu->current = current;
	pmu->power_consumption = power_consumption;
	pmu->energy_regen = energy_regen;
	pmu->energy_storage = energy_storage;
	new_sensor->sensor_type = 1;
	new_sensor->sensor_data = (power_management_unit *)pmu;
	new_sensor->nr_operations = nr_operations;
	new_sensor->operations_idxs = operations_idxs;
}

/**
	Functie folosita pentru a citi din fisierul binar senzorii si
	ai returna sortati.
	* Deschid fisierul binar si citesc din aceasta: nr_sensors, ce
	indica numarul de senzori, type-ul, ce indica tipul senzorului
	tire (type == 0) sau pmu (type == 1), cat si specificatiile
	fiecarui tip in parte.
	* Luand in calcul sortarea dupa prioritate, aleg sa folosesc doi
	vectori pentru fiecare din cele doua tipuri de senzori (sensors_pmu,
	sensors_tire).
	* Adaug fiecare element din sensors_tire in continuarea vectorului
	sensors_pmu, crescandu-i numarul de elemente, returnand sensors_pmu.
**/
sensor **read_file(char const *name_file)
{
	FILE *f = fopen(name_file, "rb");
	if (f == NULL)
		return NULL;
	int nr_sensors;
	float pressure, temperature;
	int wear_level, performace_score, nr_operations;
	float voltage, current, power_consumption;
	int energy_regen, energy_storage;
	fread(&nr_sensors, sizeof(int), 1, f);
	sensor **sensors_pmu = malloc(nr_sensors * sizeof(sensor *));
	sensor **sensors_tire = malloc(nr_sensors * sizeof(sensor *));
	int p = 0, t = 0;
	for (int i = 0; i < nr_sensors; i++) {
		int type;
		fread(&type, sizeof(int), 1, f);
		if (type == 0) {
			sensors_tire[t] = malloc(sizeof(sensor));
			sensors_tire[t]->sensor_type = type;
			fread(&pressure, sizeof(float), 1, f);
			fread(&temperature, sizeof(float), 1, f);
			fread(&wear_level, sizeof(int), 1, f);
			fread(&performace_score, sizeof(int), 1, f);
			fread(&nr_operations, sizeof(int), 1, f);
			int *operations_idx = malloc(nr_operations * sizeof(int));
			int operation;
			for (int j = 0; j < nr_operations; j++) {
				fread(&operation, sizeof(int), 1, f);
				operations_idx[j] = operation;
			}
			create_tire_sensor(sensors_tire[t], pressure, temperature,
							   wear_level, performace_score, nr_operations,
							   operations_idx);
			t++;
		} else if (type == 1) {
			sensors_pmu[p] = malloc(sizeof(sensor));
			sensors_pmu[p]->sensor_type = type;
			fread(&voltage, sizeof(float), 1, f);
			fread(&current, sizeof(float), 1, f);
			fread(&power_consumption, sizeof(float), 1, f);
			fread(&energy_regen, sizeof(int), 1, f);
			fread(&energy_storage, sizeof(int), 1, f);
			fread(&nr_operations, sizeof(int), 1, f);
			int *operations_idx = malloc(nr_operations * sizeof(int));
			for (int j = 0; j < nr_operations; j++) {
				fread(&operations_idx[j], sizeof(int), 1, f);
			}
			create_pmu(sensors_pmu[p], voltage, current, power_consumption,
					   energy_regen, energy_storage, nr_operations,
					   operations_idx);
			p++;
		}
	}
	fclose(f);
	for (int i = 0; i < t; i++) {
		sensors_pmu[p++] = sensors_tire[i];
	}
	free(sensors_tire);
	return sensors_pmu;
}

/**
	Functie folosita pentru afisarea informatiilor fiecarui
	senzor de tipul corespunzator acestuia in formatul aferent.
**/
void print_sensor(sensor *sensors)
{
	if (sensors->sensor_type == 0) {
		tire_sensor *tire = (tire_sensor *)sensors->sensor_data;
		printf("Tire Sensor\n");
		printf("Pressure: %.2f\n", tire->pressure);
		printf("Temperature: %.2f\n", tire->temperature);
		printf("Wear Level: %d%%\n", tire->wear_level);
		if (tire->performace_score > 0) {
			printf("Performance Score: %d\n", tire->performace_score);
		} else {
			printf("Performance Score: Not Calculated\n");
		}
	} else if (sensors->sensor_type == 1) {
		power_management_unit *pmu;
		pmu = (power_management_unit *)sensors->sensor_data;
		printf("Power Management Unit\n");
		printf("Voltage: %.2f\n", pmu->voltage);
		printf("Current: %.2f\n", pmu->current);
		printf("Power Consumption: %.2f\n", pmu->power_consumption);
		printf("Energy Regen: %d%%\n", pmu->energy_regen);
		printf("Energy Storage: %d%%\n", pmu->energy_storage);
	} else {
		printf("Unknown sensor type\n");
	}
}

/**
	Functie folosita pentru a efectua toate operatiile senzorului dat
	ca parametru.
	* Aici definesc un vector numit operations cu 8 elemente, fiecare
	element fiind un pointer la o functie (la o anumita operatie).
	Pentru a putea folosi functia get_operations aflata intr-un alt fisier,
	am declarat extern semnatura acesteia in acest fisier.
	* Am impartit operatia pe fiecare tip de senzor, desi nu era tocmai nevoie,
	avand in vedere ca testele primite nu aveau probleme in acest sens.
**/
void analyze(sensor *sensors)
{
	void (*operations[8])(void *);
	get_operations((void **)operations);
	for (int i = 0; i < sensors->nr_operations; i++) {
		if (sensors->sensor_type == 0 &&
			(sensors->operations_idxs[i] == 0 ||
			sensors->operations_idxs[i] == 1 ||
			sensors->operations_idxs[i] == 2 ||
			sensors->operations_idxs[i] == 3)) {
			operations[sensors->operations_idxs[i]](sensors->sensor_data);
		} else if (sensors->sensor_type == 1 &&
				   (sensors->operations_idxs[i] == 4 ||
				   sensors->operations_idxs[i] == 5 ||
				   sensors->operations_idxs[i] == 6 ||
				   sensors->operations_idxs[i] == 7)) {
			operations[sensors->operations_idxs[i]](sensors->sensor_data);
		}
	}
}

/**
	Functie folosita pentru a sterge din vector senzorii care contin
	valori eronate si a returna numarul de senzori ramasi dupa stergerea
	celor nepotriviti.
	* Parcurg senzorii si pentru fiecare tip de senzor verific conditiile
	pentru care datele acestora sunt considerate eronate.
	* In acest caz sterg si elimin senzorul respectiv din memorie,
	redimensionez vectorul de senzori si returnez nr_sensors.
	* Returnez nr_sensors pentru comanda exit in care va fi nevoie sa
	dezaloc resursele de la senzorii ramasi.
**/
int clear(sensor **sensors, int nr_sensors_orig)
{
	int nr_sensors = nr_sensors_orig;
	for (int i = 0; i < nr_sensors; i++) {
		if (sensors[i]->sensor_type == 0) {
			tire_sensor *tire = sensors[i]->sensor_data;
			if (tire->pressure < 19 || tire->pressure > 28 ||
				tire->temperature < 0 || tire->temperature > 120 ||
				tire->wear_level < 0 || tire->wear_level > 100) {
				free(tire);
				free(sensors[i]->operations_idxs);
				free(sensors[i]);
				for (int j = i; j < nr_sensors - 1; j++)
					sensors[j] = sensors[j + 1];
				nr_sensors--;
				i--;
			}
		} else if (sensors[i]->sensor_type == 1) {
			power_management_unit *pmu = sensors[i]->sensor_data;
			if (pmu->voltage < 10 || pmu->voltage > 20 ||
				pmu->power_consumption < 0 || pmu->power_consumption > 1000 ||
				pmu->current < -100 || pmu->current > 100 ||
				pmu->energy_regen < 0 || pmu->energy_regen > 100 ||
				pmu->energy_storage < 0 || pmu->energy_storage > 100) {
				free(pmu);
				free(sensors[i]->operations_idxs);
				free(sensors[i]);
				for (int j = i; j < nr_sensors - 1; j++)
					sensors[j] = sensors[j + 1];
				nr_sensors--;
				i--;
			}
		}
	}
	return nr_sensors;
}

/**
	In functia main:
	* Programul citeste o comanda de la tastatura,
	iar daca aceasta coincide cu una dintre comenzile corespunzatoare
	cerintei, continua sa citeasca indexul (acolo unde este cazul),
	apoi apeleaza functia corespunzatoare fiecarei comenzi.
	* Daca indexul nu corespunde cerintei va fi semnalata o eroare.
**/
int main(int argc, char const *argv[])
{
	char command[30];
	int idx;
	sensor **sensors;
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
		return 0;
	int nr_sensors;
	fread(&nr_sensors, sizeof(int), 1, f);
	fclose(f);
	sensors = read_file(argv[1]);
	while (1) {
		scanf("%s", command);
		if (strcmp(command, "print") == 0) {
			scanf("%d", &idx);
			if (idx >= 0 && idx < nr_sensors)
				print_sensor(sensors[idx]);
			else
				printf("Index not in range!\n");
		} else if (strcmp(command, "analyze") == 0) {
			scanf("%d", &idx);
			if (idx >= 0 && idx < nr_sensors)
				analyze(sensors[idx]);
			else
				printf("Index not in range!\n");
		} else if (strcmp(command, "clear") == 0) {
			nr_sensors = clear(sensors, nr_sensors);
		} else if (strcmp(command, "exit") == 0) {
			for (int i = nr_sensors - 1; i >= 0; i--) {
				free(sensors[i]->sensor_data);
				free(sensors[i]->operations_idxs);
				free(sensors[i]);
			}
			free(sensors);
			nr_sensors = 0;
			sensors = NULL;
			break;
		}
	}
	return 0;
}

#include "database.h"
#include <stdio.h>
#include <string.h>

// Base de datos de usuarios
static user_t users[MAX_USERS];
static uint8_t user_count = 0;

void database_init(void) {
    // Inicializar base de datos con usuarios predefinidos
    user_count = 5;
    
    // Usuario 1
    strcpy(users[0].id, "123456");
    strcpy(users[0].password, "1234");
    users[0].failed_attempts = 0;
    users[0].blocked = false;
    
    // Usuario 2
    strcpy(users[1].id, "789012");
    strcpy(users[1].password, "5678");
    users[1].failed_attempts = 0;
    users[1].blocked = false;
    
    // Usuario 3
    strcpy(users[2].id, "345678");
    strcpy(users[2].password, "9012");
    users[2].failed_attempts = 0;
    users[2].blocked = false;
    
    // Usuario 4
    strcpy(users[3].id, "901234");
    strcpy(users[3].password, "3456");
    users[3].failed_attempts = 0;
    users[3].blocked = false;
    
    // Usuario 5
    strcpy(users[4].id, "567890");
    strcpy(users[4].password, "7890");
    users[4].failed_attempts = 0;
    users[4].blocked = false;
    
    printf("Base de datos inicializada con %d usuarios\n", user_count);
}

// Buscar usuario por ID
static int find_user_by_id(const char* id) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].id, id) == 0) {
            return i;
        }
    }
    return -1; // Usuario no encontrado
}

auth_result_t authenticate_user(const char* id, const char* password) {
    int user_index = find_user_by_id(id);
    
    // Usuario no encontrado
    if (user_index == -1) {
        printf("Usuario %s no encontrado\n", id);
        return AUTH_USER_NOT_FOUND;
    }
    
    // Usuario bloqueado
    if (users[user_index].blocked) {
        printf("Usuario %s está bloqueado\n", id);
        return AUTH_USER_BLOCKED;
    }
    
    // Verificar contraseña
    if (strcmp(users[user_index].password, password) == 0) {
        // Contraseña correcta - resetear contador de intentos fallidos
        users[user_index].failed_attempts = 0;
        printf("Acceso concedido para usuario %s\n", id);
        return AUTH_SUCCESS;
    } else {
        // Contraseña incorrecta - incrementar contador
        users[user_index].failed_attempts++;
        printf("Contraseña incorrecta para usuario %s (intento %d/%d)\n", 
               id, users[user_index].failed_attempts, MAX_FAILED_ATTEMPTS);
        
        // Bloquear usuario si supera el límite
        if (users[user_index].failed_attempts >= MAX_FAILED_ATTEMPTS) {
            users[user_index].blocked = true;
            printf("Usuario %s ha sido BLOQUEADO permanentemente\n", id);
            return AUTH_USER_BLOCKED;
        }
        
        return AUTH_WRONG_PASSWORD;
    }
}

bool change_user_password(const char* id, const char* old_password, const char* new_password) {
    int user_index = find_user_by_id(id);
    
    if (user_index == -1 || users[user_index].blocked) {
        return false;
    }
    
    // Verificar contraseña actual
    if (strcmp(users[user_index].password, old_password) == 0) {
        strcpy(users[user_index].password, new_password);
        printf("Contraseña cambiada exitosamente para usuario %s\n", id);
        return true;
    }
    
    return false;
}

void print_database_status(void) {
    printf("\n=== ESTADO DE LA BASE DE DATOS ===\n");
    for (int i = 0; i < user_count; i++) {
        printf("Usuario %d: ID=%s, Intentos fallidos=%d, Bloqueado=%s\n",
               i+1, users[i].id, users[i].failed_attempts,
               users[i].blocked ? "SÍ" : "NO");
    }
    printf("================================\n\n");
}

/**
 * @file database.h
 * @brief Base de datos de usuarios para sistema de control de acceso
 * @author Sistema de Control de Acceso
 * @date 2025
 * 
 * Este módulo maneja la base de datos de usuarios del sistema, incluyendo
 * autenticación, bloqueo por intentos fallidos y cambio de contraseñas.
 * La base de datos se almacena en memoria RAM y se inicializa con usuarios
 * predeterminados.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Número máximo de usuarios en la base de datos */
#define MAX_USERS 10

/** @brief Longitud del ID de usuario (6 dígitos) */
#define ID_LENGTH 6

/** @brief Longitud de la contraseña (4 dígitos) */
#define PASSWORD_LENGTH 4

/** @brief Número máximo de intentos fallidos antes de bloqueo */
#define MAX_FAILED_ATTEMPTS 3

/**
 * @brief Estructura que representa un usuario en la base de datos
 */
typedef struct {
    char id[ID_LENGTH + 1];        /**< ID de 6 dígitos + terminador nulo */
    char password[PASSWORD_LENGTH + 1]; /**< Contraseña de 4 dígitos + terminador nulo */
    uint8_t failed_attempts;       /**< Contador de intentos fallidos consecutivos */
    bool blocked;                  /**< Estado de bloqueo del usuario */
} user_t;

/**
 * @brief Resultados posibles de la autenticación de usuario
 */
typedef enum {
    AUTH_SUCCESS,           /**< Autenticación exitosa */
    AUTH_USER_NOT_FOUND,    /**< ID de usuario no encontrado en la base de datos */
    AUTH_WRONG_PASSWORD,    /**< Contraseña incorrecta */
    AUTH_USER_BLOCKED       /**< Usuario bloqueado por intentos fallidos */
} auth_result_t;

/**
 * @brief Inicializa la base de datos con usuarios predeterminados
 * 
 * Carga la base de datos con 5 usuarios de prueba:
 * - ID: "123456", Password: "1234"
 * - ID: "789012", Password: "5678"
 * - ID: "345678", Password: "9012"
 * - ID: "901234", Password: "3456"
 * - ID: "567890", Password: "7890"
 * 
 * Todos los usuarios inician sin bloqueos ni intentos fallidos.
 */
void database_init(void);

/**
 * @brief Autentica un usuario con ID y contraseña
 * 
 * Busca el usuario en la base de datos y verifica su contraseña.
 * Maneja el conteo de intentos fallidos y bloqueo automático.
 * 
 * @param id ID del usuario (6 dígitos como string)
 * @param password Contraseña del usuario (4 dígitos como string)
 * 
 * @return auth_result_t Resultado de la autenticación
 * 
 * @note Si la autenticación es exitosa, se resetea el contador de intentos fallidos
 * @note Al superar MAX_FAILED_ATTEMPTS, el usuario se bloquea permanentemente
 */
auth_result_t authenticate_user(const char* id, const char* password);

/**
 * @brief Cambia la contraseña de un usuario
 * 
 * Verifica la contraseña actual del usuario y la reemplaza por una nueva.
 * Solo funciona si el usuario existe, no está bloqueado, y la contraseña
 * actual es correcta.
 * 
 * @param id ID del usuario
 * @param old_password Contraseña actual del usuario
 * @param new_password Nueva contraseña a establecer
 * 
 * @return true Si el cambio fue exitoso
 * @return false Si el cambio falló (usuario no encontrado, bloqueado, o contraseña incorrecta)
 */
bool change_user_password(const char* id, const char* old_password, const char* new_password);

/**
 * @brief Imprime el estado actual de todos los usuarios en la base de datos
 * 
 * Función de diagnóstico que muestra información de todos los usuarios:
 * ID, número de intentos fallidos, y estado de bloqueo.
 * 
 * @note Solo para propósitos de depuración y diagnóstico
 */
void print_database_status(void);

#endif // DATABASE_H

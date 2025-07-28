/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "My Project", "index.html", [
    [ "Sistema de Control de Acceso con Raspberry Pi Pico", "index.html", "index" ],
    [ "README_DOXYGEN", "md__r_e_a_d_m_e___d_o_x_y_g_e_n.html", null ],
    [ "Sistema de Control de Acceso con FreeRTOS y Display SSD1306", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html", [
      [ "Descripción del Proyecto", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md1", null ],
      [ "Características del Sistema", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md2", [
        [ "Hardware Requerido", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md3", null ],
        [ "Conexiones de Hardware", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md4", [
          [ "Teclado Matricial 4x4", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md5", null ],
          [ "LEDs de Señalización", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md6", null ],
          [ "Display SSD1306 I2C", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md7", null ]
        ] ]
      ] ],
      [ "Arquitectura del Sistema", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md8", [
        [ "Tareas de FreeRTOS", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md9", null ],
        [ "Comunicación Entre Tareas", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md10", null ]
      ] ],
      [ "Funcionalidad del Display SSD1306", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md11", [
        [ "Mensajes del Sistema", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md12", null ]
      ] ],
      [ "Señalización LED", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md13", [
        [ "Patrones de LEDs", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md14", null ]
      ] ],
      [ "Base de Datos de Usuarios", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md15", [
        [ "Usuarios Predeterminados", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md16", null ],
        [ "Funcionalidades de Seguridad", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md17", null ]
      ] ],
      [ "Compilación y Uso", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md18", [
        [ "Requisitos de Software", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md19", null ],
        [ "Comandos de Compilación", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md20", null ],
        [ "Archivos Principales", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md21", null ]
      ] ],
      [ "Uso del Sistema", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md22", [
        [ "Proceso de Autenticación Normal", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md23", null ],
        [ "Cambio de Contraseña", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md24", null ],
        [ "Cancelación", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md25", null ]
      ] ],
      [ "Características Técnicas de FreeRTOS", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md26", [
        [ "Configuración del Kernel", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md27", null ],
        [ "Optimizaciones", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md28", null ]
      ] ],
      [ "Manejo de Errores", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md29", [
        [ "Hooks de FreeRTOS", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md30", null ],
        [ "Señalización de Errores", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md31", null ]
      ] ],
      [ "Futuras Mejoras", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md32", null ],
      [ "Autor", "md__r_e_a_d_m_e___p_r_o_y_e_c_t_o2.html#autotoc_md33", null ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_free_r_t_o_s_config_8h.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';
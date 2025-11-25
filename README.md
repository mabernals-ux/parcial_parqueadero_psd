# Parqueadero Automatizado

## Descripción

El **Parqueadero Automatizado** es un sistema integral que combina **interfaces web, base de datos, hardware y software** para gestionar de manera eficiente los espacios de estacionamiento. Permite controlar entradas o salidas de vehículos, asignar espacios además mantener un registro actualizado de usuarios y vehículos.  

El sistema está diseñado para dos tipos de usuarios: **administradores** y **usuarios normales**, cada uno con distintas funcionalidades.

---

## Funcionalidades

### Interfaz de Administrador
La interfaz de administrador permite:  
- **Registrar Usuario:** Añadir nuevos usuarios al sistema.  
- **Registrar Vehículo:** Asociar vehículos a usuarios registrados.  
- **Asignar Espacio:** Asignar un puesto disponible a un vehículo.  
- **Registrar Salida:** Registrar la salida de un vehículo del parqueadero.  
- **Estado:** Consultar el estado actual de los puestos (ocupados o libres).  
- **Listados:** Generar listados de usuarios, vehículos, registro, registros de recargas.  
- **Recargar:** Gestionar recargas o pagos relacionados al parqueadero.  
- **Cambiar Rol:** Cambiar el rol de un usuario dentro del sistema.  

### Interfaz de Usuario
Los usuarios normales pueden:  
- **Registrar Usuario**  
- **Registrar Vehículo**
- **Cambiar Rol** 

Todas las solicitudes de la interfaz web se envían al **backend desarrollado en Python con Flask**, que se encarga de procesarlas y almacenar la información en una **base de datos SQL**.

---

## Integración con Hardware

El sistema cuenta con un componente de hardware formado por **ESP32 y Arduino**:  
- La **ESP32** se comunica con el servidor Python enviando información sobre el estado del parqueadero y lecturas de **RFID**.  
- El **Arduino**, conectado a la ESP32 mediante un protocolo maestro-esclavo, monitorea los **puestos ocupados y libres**.  
- Las interacciones con **tarjetas RFID** permiten registrar de manera automática las **entradas y salidas** de los vehículos.  

Esta integración permite que la información en tiempo real del parqueadero se refleje en la interfaz web y automatiza la gestión de los espacios.

---

## Tecnologías Utilizadas

- **Frontend:** HTML, javaScript
- **Backend:** Python, Flask  
- **Base de Datos:** SQL
- **Hardware:** ESP32, Arduino, módulo RFID  



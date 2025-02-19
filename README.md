Genera una lista gigante de nombres (ejemplo: Ana1, Carlos2, Beatriz3, ...).
Divide la lista entre 8 hilos, cada uno buscando en su propia parte.
Si un hilo encuentra el nombre, detiene a los demás.
Un supervisor monitorea el proceso e informa cada segundo.
Al final, muestra si se encontró el nombre o no.
Se implementa sincronizacion por barrier y uso de tareas asincronas

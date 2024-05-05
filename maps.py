#!/usr/bin/env python3

import geopandas as gpd
import matplotlib.pyplot as plt

def plot_timezones(geojson_path):
    # Load the GeoJSON file into a GeoDataFrame
    timezones = gpd.read_file(geojson_path)
    
    # Create a plot of the timezones
    fig, ax = plt.subplots(1, 1, figsize=(20, 10))
    timezones.plot(ax=ax, cmap='tab20b', edgecolor='black')  # Using a colormap and edgecolor for better visibility
    
    # Set plot title and labels
    ax.set_title('World Time Zones Map')
    ax.set_xlabel('Longitude')
    ax.set_ylabel('Latitude')
    
    # Remove x and y axis for a cleaner look
    ax.set_xticks([])
    ax.set_yticks([])
    
    # Show the plot
    plt.show()

# Replace 'path_to_timezones_geojson' with the path to your timezones GeoJSON file
plot_timezones('./timezones.json')
